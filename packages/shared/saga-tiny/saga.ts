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
  SelectEffect,
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
  FINISH,
  SELECT,
} from './private/symbols';
import genId from '../genId';
import EventEmitter, { Listener, Connection } from './eventEmitter';
import { tostr, zip } from '../functional';

const id = genId();

type Method = typeof Next | typeof Throw | typeof Return;
const Next = 'next';
const Throw = 'throw';
const Return = 'return';

type Response = any;

function assertEffect(effect: any): effect is Effect {
  if (!effect || !(effectType in effect)) {
    throw new Error(
      `Saga: A Effect was expected but the value is: ${tostr(effect)}`
    );
  }

  return effect;
}

export type TaskContext = {
  actionSubscriber: EventEmitter;
  commit: (type: string, ...payload: any[]) => void;
  getState: () => any;
};

class TaskController implements Task {
  m_taskId = `@SAGA_TASK_${id.next().value}`;
  m_parent: TaskController | null;
  m_status: TaskStatus = TaskStatus.Running;
  m_result?: any;

  saga: Saga;
  subTasks: Record<string, TaskController> = {};
  emitter = new EventEmitter();
  connections: Connection[] = []; // subscriptions
  context: TaskContext;

  constructor(context: TaskContext, parent: TaskController | null, saga: Saga) {
    this.m_parent = parent;
    if (parent) {
      parent.subTasks[this.m_taskId] = this;
    }
    this.saga = saga;
    this.context = context;
  }

  run(): TaskController {
    Qt.callLater(this.advancer.bind(this));
    return this;
  }

  protected advancer(response?: Response, requestStatus?: TaskStatus) {
    let action: Method;
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
      const result = this.saga[action](response);
      this.handleNext(result);
    } catch (e) {
      console.error(e.name, e.message, e.stack);
      if (this.m_status !== TaskStatus.Aborted) {
        this.m_status = TaskStatus.Aborted;
        this.releaseContextConnections();
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
      this.releaseContextConnections();
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
    this.releaseContextConnections();
    for (let id in this.subTasks) {
      this.subTasks[id].cancel();
    }
    this.subTasks = {};
  }

  notifyFinish() {
    this.emitter.emit(FINISH, this.m_result, this.m_status);
    this.emitter.offAll();
  }

  releaseContextConnections() {
    this.context.actionSubscriber.offAll(this.connections);
  }

  deleteSubTask(task: TaskController) {
    task.m_parent = null;
    delete this.subTasks[task.taskId];
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
  [SELECT](this: TaskController, effect: SelectEffect) {
    this.advancer(effect.selector(this.context.getState()));
  },
  [TAKE](this: TaskController, effect: TakeEffect) {
    const { pattern } = effect;
    this.connections.push(
      this.context.actionSubscriber.once(pattern, payload => {
        this.advancer(payload);
      })
    );
  },
  [PUT](this: TaskController, effect: PutEffect) {
    const { type, args } = effect;
    this.context.commit(type, ...args);
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
  parent: TaskController,
  saga: Fn,
  args: Parameters<Fn>
) {
  return new TaskController(parent.context, parent, saga(...args));
}

export function runSaga<Fn extends SagaFn>(
  context: TaskContext,
  saga: Fn,
  ...args: Parameters<Fn>
): Task {
  return new TaskController(context, null, saga(...args)).run();
}
