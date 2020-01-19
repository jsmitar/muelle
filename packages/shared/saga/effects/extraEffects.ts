import Qt from 'qt';
import { Action } from '../../flux/flux';
import { TaskStatus } from '../private/symbols';
import { Saga, SagaFn, Task } from '../private/types';
import { call, cancel, delayed, fork, forkNamed, take } from './baseEffects';

export function delay(ms: number) {
  return delayed(resolve => {
    const timer = Qt.setTimeout(resolve, ms);
    return () => Qt.clearTimeout(timer);
  });
}

export function takeLeading<Fn extends SagaFn>(
  pattern: string,
  saga: Fn,
  ...args: Parameters<Fn>
) {
  return forkNamed(function*(): Saga {
    while (true) {
      const action: Action = yield take(pattern);
      yield call(saga, ...(args.concat(action) as any));
    }
  }, saga.name);
}

export function takeLatest<Fn extends SagaFn>(
  pattern: string,
  saga: Fn,
  ...args: Parameters<Fn>
) {
  return forkNamed(function*(): Saga {
    let lastTask: Task | undefined;
    while (true) {
      const action: Action = yield take(pattern);
      if (lastTask && lastTask.status === TaskStatus.Running)
        yield cancel(lastTask);
      lastTask = yield fork(saga, ...(args.concat(action) as any));
    }
  }, saga.name);
}

export function takeEvery<Fn extends SagaFn>(
  pattern: string,
  saga: Fn,
  ...args: Parameters<Fn>
) {
  return forkNamed(function*(): Saga {
    while (true) {
      const action: Action = yield take(pattern);
      yield fork(saga, ...(args.concat(action) as any));
    }
  }, saga.name);
}
