import '../../types/Qt';
import {
  Saga,
  ITask,
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
  ITaskPrivate,
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
import { EventEmitter, Listener, Connection } from './EventEmitter';
import Qt from 'qt';
import { AssertionError } from 'assert';

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
    throw new AssertionError({
      message: 'Saga internal error',
      expected: 'Effect',
      actual: effect,
    });
  }

  return effect;
}

class TaskController implements ITaskPrivate {
  m_taskId = `@SAGA_TASK_${id.next().value}`;
  m_parent: TaskController | null;
  m_saga: Saga;
  m_subTasks: Record<string, ITaskPrivate> = {};
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

  run(): ITask {
    Qt.callLater(this.advancer);
    return this;
  }

  protected advancer(response?: Response, requestStatus?: TaskStatus) {
    let action: Action;

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
      if (this.m_status !== TaskStatus.Aborted) {
        this.releaseSagaConnections();
        this.propagateAbort(e);
        this.notifyFinish();
      }
    }
  }

  handleNext({ value, done }: IteratorResult<Effect, unknown>) {
    if (done) {
      if (assertEffect(value)) {
        effectHandlers[value[effectType]].call<TaskController, any[], void>(
          this,
          value
        );
      }
    } else {
      this.releaseSagaConnections();
      this.notifyFinish();
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
      const subtask = this.m_subTasks[id];
      if (subtask.status === TaskStatus.Running) {
        this.m_subTasks[id].cancel();
      }
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

  deleteSubTask(task: ITaskPrivate) {
    task.m_parent = null;
    delete this.m_subTasks[task.taskId];
  }

  cancel() {
    this.advancer(undefined, TaskStatus.Cancelled);
  }

  abort(e: Error) {
    this.advancer(e, TaskStatus.Aborted);
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

  get task(): ITask {
    return this;
  }
}

const effectHandlers = {
  [CALL](this: TaskController, effect: CallEffect) {
    const task = new TaskController(this, effect.saga(...effect.args));
    task.onFinish(this.advancer);
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
    const clear = effect.delayed(result => {
      clear();
      this.emitter.off(conn);
      if (result instanceof Error) {
        this.advancer(result, TaskStatus.Aborted);
      } else {
        this.advancer(result);
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
    task.onFinish(this.advancer);
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
        if (countdown === 0) {
          this.advancer(results);
        } else {
          if (status === TaskStatus.Done) {
            --countdown;
            results[index] = result;
          } else {
            tasks.forEach(task => task.cancel());
            this.advancer(undefined, status);
          }
        }
      };
    };

    tasks.forEach((task, index) => {
      task.onFinish(finishHandler(index));
      task.run();
    });
  },
  [RACE](this: TaskController, effect: RaceEffect) {
    const { effects, count } = effect;

    const results = Array.from({ length: count });
    const tasks = effects.map(({ saga, args }) => makeTask(this, saga, args));

    const finishHandler = (index: number): Listener<[Response, TaskStatus]> => {
      return (result, status) => {
        results[index] = result;
        tasks.forEach(task => task.cancel());
        this.advancer(results, status);
      };
    };
    tasks.forEach((task, index) => {
      task.onFinish(finishHandler(index));
      task.run();
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

export function run<Fn extends SagaFn>(saga: Fn, ...args: Parameters<Fn>) {
  return makeTask(null, saga, args).run();
}
