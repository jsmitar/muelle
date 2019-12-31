import 'qt.globals';
import { Saga, ITask, SagaFn, Effect } from './types';
import { TaskStatus } from './private/symbols';
import genId from './private/genId';
import { EventEmitter } from './EventEmitter';

const id = genId();

type Action = 'next' | 'throw' | 'return';
const Next = 'next';
const Throw = 'throw';
const Return = 'return';

class TaskController implements ITask {
  m_taskId = `@SAGA_TASK_${id.next().value}`;
  m_parent: TaskController | null;
  m_saga: Saga;
  m_subTasks: Record<string, ITask> = {};
  m_status: TaskStatus = TaskStatus.Running;
  m_result?: any;
  emitter = new EventEmitter();

  constructor(parent: TaskController | null, saga: Saga) {
    this.m_parent = parent;
    this.m_saga = saga;
  }

  run(): ITask {
    Qt.callLater(this.advancer);
    return this;
  }

  private advancer(value?: any, request?: TaskStatus) {
    let action: Action = Next;

    if (request === TaskStatus.Cancelled) {
      this.m_status = request;
      action = Return;
    } else if (request === TaskStatus.Aborted) {
      action = Throw;
    }

    try {
      const effect: Effect = this.m_saga[action](value).value;
      this.handleEffect(effect);
    } catch (e) {
      if (this.m_status !== TaskStatus.Aborted) {
        this.m_status = TaskStatus.Aborted;
        this.m_parent?.abort(e);
        this.release();
      }
    }
  }

  handleEffect(_effect: Effect) {}

  release() {
    this.emitter.offAll();
    this.m_parent?.deleteSubTask(this);
    this.m_parent = null;
  }

  deleteSubTask(task: ITask) {
    delete this.m_subTasks[task.taskId];
  }

  cancel() {
    this.advancer(undefined, TaskStatus.Cancelled);
  }

  abort(e: Error) {
    this.advancer(e, TaskStatus.Aborted);
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

export function run<T extends SagaFn>(saga: T, ...args: Parameters<T>) {
  return new TaskController(null, saga(...args)).run();
}
