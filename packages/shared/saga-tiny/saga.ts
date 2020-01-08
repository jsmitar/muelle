import Qt from 'qt';
import {
  Saga,
  Task,
  SagaFn,
  Effect,
  CallEffect,
  TakeEffect,
  PutEffect,
  DelayedEffect,
  ForkEffect,
  JoinEffect,
  CancelEffect,
  CancelledEffect,
  AllEffect,
  RaceEffect,
} from './private/types';
import {
  TaskStatus,
  CALL,
  TAKE,
  PUT,
  ALL,
  CANCEL,
  CANCELLED,
  DELAYED,
  FORK,
  JOIN,
  RACE,
  effectType,
} from './private/symbols';
import genId from '../genId';
import EventEmitter, { Listener, Connection } from './eventEmitter';
import { tostr, zip } from '../functional';

const id = genId();

type Action = 'next' | 'throw' | 'return';
const Next = 'next';
const Throw = 'throw';
const Return = 'return';

const FINISH = '@saga/finish';

type Response = any;

export const sagaEmitter = new EventEmitter();

function assertEffect(effect: any): effect is Effect {
  if (!effect || !(effectType in effect)) {
    throw new Error(
      `Saga: A Effect was expected but the value is: ${tostr(effect)}`
    );
  }

  return effect;
}

class TaskController implements Task {
  m_taskId = `@SAGA_TASK_${id.next().value}`;
  m_parent: TaskController | null;
  m_saga: Saga;
  m_subTasks: Record<string, TaskController> = {};
  m_status: TaskStatus = TaskStatus.Running;
  m_result?: any;
  emitter = new EventEmitter();
  conn: Connection[] = []; // Saga Connections

  constructor(parent: TaskController | null, saga: Saga) {
    this.m_parent = parent;
    if (parent) {
      parent.m_subTasks[this.m_taskId] = this;
    }
    this.m_saga = saga;
  }

  run(): TaskController {
    Qt.callLater(this.advancer.bind(this));
    return this;
  }

  protected advancer(response?: Response, requestStatus?: TaskStatus) {
    let action: Action;
    //console.info('response:', tostr(response), requestStatus || '');

    switch (requestStatus) {
      case TaskStatus.Cancelled:
        this.propagateCancel();
        response = undefined;
        action = Return;
        break;
      case TaskStatus.Aborted:
        action = Throw;
        break;
      default:
        action = Next;
    }

    try {
      const result = this.m_saga[action](response);
      this.handleNext(result);
    } catch (e) {
      console.error(e.name, e.message, e.stack);
      if (this.m_status !== TaskStatus.Aborted) {
        this.m_status = TaskStatus.Aborted;
        this.releaseSagaConnections();
        this.propagateAbort(e);
        this.notifyFinish();
      }
    }
  }

  handleNext({ value, done }: IteratorResult<Effect, unknown>) {
    if (!done) {
      if (assertEffect(value)) {
        effectHandlers[value[effectType]].call<TaskController, any[], void>(
          this,
          value
        );
      }
    } else {
      this.releaseSagaConnections();
      if (this.m_status === TaskStatus.Running) {
        this.m_status = TaskStatus.Done;
        this.m_result = value;
        this.notifyFinish();
      }
      this.deleteSubTask(this);
    }
  }

  propagateAbort(error: Error) {
    this.m_status = TaskStatus.Aborted;
    this.m_parent?.abort(error);
    this.m_parent?.deleteSubTask(this);
  }

  propagateCancel() {
    this.m_status = TaskStatus.Cancelled;
    this.releaseSagaConnections();
    for (let id in this.m_subTasks) {
      this.m_subTasks[id].cancel();
    }
    this.m_subTasks = {};
  }

  notifyFinish() {
    this.emitter.emit(FINISH, this.m_result, this.m_status);
    this.emitter.offAll();
  }

  releaseSagaConnections() {
    sagaEmitter.offAll(this.conn);
  }

  deleteSubTask(task: TaskController) {
    task.m_parent = null;
    delete this.m_subTasks[task.taskId];
  }

  cancel() {
    if (this.m_status === TaskStatus.Running) {
      this.advancer(undefined, TaskStatus.Cancelled);
    }
  }

  abort(e: Error) {
    if (this.m_status === TaskStatus.Running) {
      this.advancer(e, TaskStatus.Aborted);
    }
  }

  onFinish(listener: Listener<[Response, TaskStatus]>) {
    return this.emitter.once(FINISH, listener);
  }

  get taskId() {
    return this.m_taskId;
  }

  get result() {
    return this.m_result;
  }

  get status() {
    return this.m_status;
  }

  get task(): Task {
    return this;
  }
}

const effectHandlers = {
  [CALL](this: TaskController, effect: CallEffect) {
    const task = makeTask(this, effect.saga, effect.args);
    task.onFinish(this.advancer.bind(this));
    task.run();
  },
  [TAKE](this: TaskController, effect: TakeEffect) {
    const { pattern } = effect.action;
    this.conn.push(
      sagaEmitter.once(pattern, payload => {
        this.advancer(payload);
      })
    );
  },
  [PUT](this: TaskController, effect: PutEffect) {
    const { type, payload } = effect.action;
    sagaEmitter.emit(type, payload);
    this.advancer();
  },
  [DELAYED](this: TaskController, effect: DelayedEffect) {
    const self = this;
    const clear = effect.delayed(result => {
      clear();
      self.emitter.off(conn);
      if (result instanceof Error) {
        self.advancer(result, TaskStatus.Aborted);
      } else {
        self.advancer(result);
      }
    });

    const conn = this.onFinish(clear);
  },
  [FORK](this: TaskController, effect: ForkEffect) {
    const task = makeTask(this, effect.saga, effect.args).run();
    this.advancer(task);
  },
  [JOIN](this: TaskController, effect: JoinEffect) {
    const task = <TaskController>effect.task;
    task.onFinish(this.advancer.bind(this));
  },
  [CANCEL](this: TaskController, effect: CancelEffect) {
    const task = <TaskController>effect.task || this;
    task.cancel();
  },
  [CANCELLED](this: TaskController, _: CancelledEffect) {
    this.advancer(this.status === TaskStatus.Cancelled);
  },
  [ALL](this: TaskController, effect: AllEffect) {
    const { effects, count } = effect;

    let countdown = count;
    const results = Array.from({ length: count });

    const tasks = effects.map(({ saga, args }) => makeTask(this, saga, args));

    const finishHandler = (index: number): Listener<[Response, TaskStatus]> => {
      return (result, status) => {
        --countdown;
        if (status === TaskStatus.Done) {
          results[index] = result;
        } else {
          countdown = 0;
        }

        if (countdown === 0) {
          zip(tasks, conns).forEach(([task, conn]) => {
            task.emitter.off(conn);
            task.cancel();
          });

          this.advancer(results, status);
        }
      };
    };

    const conns = tasks.map((task, index) => {
      task.run();
      return task.onFinish(finishHandler(index).bind(this));
    });
  },
  [RACE](this: TaskController, effect: RaceEffect) {
    const { effects, count } = effect;

    let countdown = count;
    const results = Array.from({ length: count });

    const tasks = effects.map(({ saga, args }) => makeTask(this, saga, args));

    const finishHandler = (index: number): Listener<[Response, TaskStatus]> => {
      return (result, status) => {
        --countdown;
        if (countdown === 0 || status === TaskStatus.Done) {
          results[index] = result;
          zip(tasks, conns).forEach(([task, conn]) => {
            task.emitter.off(conn);
            task.cancel();
          });
          this.advancer(results);
        }
      };
    };
    const conns = tasks.map((task, index) => {
      task.run();
      return task.onFinish(finishHandler(index).bind(this));
    });
  },
};

export function makeTask<Fn extends SagaFn>(
  parent: TaskController | null,
  saga: Fn,
  args: Parameters<Fn>
) {
  return new TaskController(parent, saga(...args));
}

export function run<Fn extends SagaFn>(
  saga: Fn,
  ...args: Parameters<Fn>
): Task {
  return makeTask(null, saga, args).run();
}
