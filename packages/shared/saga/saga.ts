import { ActionAny } from 'qrc:/shared/flux/flux';
import { isEmpty, noop, zip } from 'qrc:/shared/functional';
import genId from 'qrc:/shared/genId';
import EventEmitter, {
  Connection,
  Listener,
} from 'qrc:/shared/saga/eventEmitter';
import { isEffect } from 'qrc:/shared/saga/helpers';
import {
  ALL,
  CALL,
  CANCEL,
  CANCELLED,
  COMMIT,
  DELAYED,
  effectType,
  FINISH,
  FORK,
  JOIN,
  PUT,
  RACE,
  SELECT,
  TAKE,
  TaskStatus,
} from 'qrc:/shared/saga/private/symbols';
import {
  AllEffect,
  CallEffect,
  CancelEffect,
  CancelledEffect,
  CommitEffect,
  DelayedEffect,
  Effect,
  ForkEffect,
  JoinEffect,
  PutEffect,
  RaceEffect,
  Saga,
  SagaFn,
  SelectEffect,
  TakeEffect,
  Task,
} from 'qrc:/shared/saga/private/types';
import { SagaMonitorHooks } from 'qrc:/shared/saga/sagaMonitor';
import Qt from 'qt';

const id = genId();

type Method = typeof Next | typeof Throw | typeof Return;
const Next = 'next';
const Throw = 'throw';
const Return = 'return';

type Response = any;

export type TaskContext = {
  sagaMonitor?: SagaMonitorHooks;
  actionSubscriber: EventEmitter;
  dispatch(action: ActionAny): void;
  commit(type: string, ...args: any[]): void;
  getState(): any;
};

export class TaskController implements Task {
  m_taskId = `@SAGA_TASK_${id.next().value}`;
  m_name = '';
  m_parent: TaskController | null;
  m_status: TaskStatus = TaskStatus.Running;
  m_result?: any;

  saga: Saga;
  subTasks: Record<string, TaskController> = {};
  emitter = new EventEmitter();
  connections: Connection[] = []; // subscriptions
  context: Required<TaskContext>;

  constructor(
    context: Required<TaskContext>,
    parent: TaskController | null,
    saga: Saga,
    name: string
  ) {
    this.m_parent = parent;
    this.m_name = name;
    if (parent) {
      parent.subTasks[this.m_taskId] = this;
    }
    this.saga = saga;
    this.context = context;
  }

  run(delayed = true): TaskController {
    if (delayed) {
      Qt.callLater(this.run.bind(this, false));
    } else {
      this.context.sagaMonitor.statusChanged(this);
      this.advancer();
    }
    return this;
  }

  protected advancer(
    effect?: Effect,
    response?: Response,
    requestStatus: TaskStatus = TaskStatus.Running
  ) {
    let action: Method;
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

    if (effect) this.context.sagaMonitor.effectResolved(this, effect, response);

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
      if (isEffect(value)) {
        this.context.sagaMonitor.effectTriggered(this, value);
        effectHandlers[value[effectType]].call<TaskController, any[], void>(
          this,
          value
        );
      }
    } else {
      this.m_result = value;
      if (this.m_status === TaskStatus.Running) {
        this.m_status = TaskStatus.Done;
      }
      this.releaseContextConnections();
      this.handleFinish();
    }
  }

  handleFinish() {
    if (isEmpty(this.subTasks)) {
      this.notifyFinish();
      this.m_parent?.deleteSubTask(this);
    }
  }

  propagateAbort(error: Error) {
    this.m_status = TaskStatus.Aborted;
    this.m_parent?.abort(error);
    this.m_parent?.deleteSubTask(this);
  }

  propagateCancel() {
    this.m_status = TaskStatus.Cancelled;
    for (let id in this.subTasks) {
      this.subTasks[id].cancel();
    }
    this.subTasks = {};
    this.m_parent?.deleteSubTask(this);
  }

  notifyFinish() {
    this.context.sagaMonitor.statusChanged(this, this.m_result);
    this.emitter.emit(FINISH, this.m_result, this.m_status);
    this.emitter.offAll();
  }

  releaseContextConnections() {
    this.context.actionSubscriber.offAll(this.connections);
    this.connections = [];
  }

  deleteSubTask(task: TaskController) {
    task.m_parent = null;
    delete this.subTasks[task.taskId];

    this.handleFinish();
  }

  cancel(effect?: CancelEffect) {
    if (effect && effect.task && effect.task !== this) {
      (<TaskController>effect.task).cancel();
      this.advancer(effect);
    } else if (this.m_status === TaskStatus.Running) {
      this.advancer(effect, undefined, TaskStatus.Cancelled);
    }
  }

  abort(e: Error) {
    if (this.m_status === TaskStatus.Running) {
      this.advancer(undefined, e, TaskStatus.Aborted);
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

  get name() {
    return this.m_name;
  }

  [Symbol.toPrimitive]() {
    const start = '\u001b[36m';
    const end = '\u001b[0m';
    const name = this.m_name === 'saga' ? '' : ` ${this.m_name}`;
    return `${start}[${this.m_taskId}${name}]${end}`;
  }
}

const effectHandlers = {
  [CALL](this: TaskController, effect: CallEffect) {
    const task = makeTask(this, effect.saga, effect.name, effect.args);
    task.onFinish(this.advancer.bind(this, effect));
    task.run();
  },
  [SELECT](this: TaskController, effect: SelectEffect) {
    this.advancer(effect, effect.selector(this.context.getState()));
  },
  [TAKE](this: TaskController, effect: TakeEffect) {
    const { pattern } = effect;
    const conn = this.context.actionSubscriber.once(pattern, payload => {
      const i = this.connections.findIndex(c => c.id === conn.id);
      if (i !== -1) this.connections.splice(i, 1);
      this.advancer(effect, payload);
    });
    this.connections.push(conn);
  },
  [PUT](this: TaskController, effect: PutEffect) {
    this.context.dispatch(effect);
    this.advancer(effect);
  },
  [COMMIT](this: TaskController, effect: CommitEffect) {
    const { type, args } = effect;

    this.context.commit(type, ...args);
    this.advancer(effect);
  },
  [DELAYED](this: TaskController, effect: DelayedEffect) {
    const self = this;
    const clear = effect.delayed(result => {
      clear();
      self.emitter.off(conn);
      if (result instanceof Error) {
        self.advancer(effect, result, TaskStatus.Aborted);
      } else {
        self.advancer(effect, result);
      }
    });

    const conn = this.onFinish(clear);
  },
  [FORK](this: TaskController, effect: ForkEffect) {
    const task = makeTask(this, effect.saga, effect.name, effect.args).run();
    this.advancer(effect, task);
  },
  [JOIN](this: TaskController, effect: JoinEffect) {
    const task = <TaskController>effect.task;
    task.onFinish(this.advancer.bind(this, effect));
  },
  [CANCEL](this: TaskController, effect: CancelEffect) {
    this.cancel(effect);
  },
  [CANCELLED](this: TaskController, effect: CancelledEffect) {
    this.advancer(effect, this.status === TaskStatus.Cancelled);
  },
  [ALL](this: TaskController, effect: AllEffect) {
    const { effects, count } = effect;

    let countdown = count;
    const results = Array.from({ length: count });

    const tasks = effects.map(({ saga, args, name }) =>
      makeTask(this, saga, name, args)
    );

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

          this.advancer(effect, results, status);
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

    const tasks = effects.map(({ saga, args, name }) =>
      makeTask(this, saga, name, args)
    );

    const finishHandler = (index: number): Listener<[Response, TaskStatus]> => {
      return (result, status) => {
        --countdown;
        if (countdown === 0 || status === TaskStatus.Done) {
          results[index] = result;
          zip(tasks, conns).forEach(([task, conn]) => {
            task.emitter.off(conn);
            task.cancel();
          });
          this.advancer(effect, results);
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
  name: string,
  args: Parameters<Fn>
) {
  return new TaskController(parent.context, parent, saga(...args), name);
}

export function runSaga<Fn extends SagaFn>(
  context: TaskContext,
  saga: Fn | { saga: Fn; name: string; onFinish: Listener<[any, TaskStatus]> },
  ...args: Parameters<Fn>
): Task {
  if (!context.sagaMonitor) {
    context.sagaMonitor = {
      effectResolved: noop,
      effectTriggered: noop,
      rootSagaStarted: noop,
      statusChanged: noop,
    };
  }

  const task = new TaskController(
    <Required<TaskContext>>context,
    null,
    'saga' in saga ? saga.saga(...args) : saga(...args),
    'name' in saga ? saga.name : 'root'
  );

  if ('onFinish' in saga) {
    task.onFinish(saga.onFinish);
  }

  return task.run(/* delayed */ false);
}
