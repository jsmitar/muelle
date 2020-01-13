import Qt from 'qt';
import { delayed, fork, cancel, take, call } from './baseEffects';
import { Task, SagaFn, Saga } from '../private/types';
import { Action } from '../../flux/flux';

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
  return fork(function*(): Saga {
    while (true) {
      const action: Action = yield take(pattern);
      yield call(saga, ...(args.concat(action) as any));
    }
  });
}

export function takeLatest<Fn extends SagaFn>(
  pattern: string,
  saga: Fn,
  ...args: Parameters<Fn>
) {
  return fork(function*(): Saga {
    let lastTask: Task | undefined;
    while (true) {
      const action: Action = yield take(pattern);
      if (lastTask) yield cancel(lastTask);
      lastTask = yield fork(saga, ...(args.concat(action) as any));
    }
  });
}

export function takeEvery<Fn extends SagaFn>(
  pattern: string,
  saga: Fn,
  ...args: Parameters<Fn>
) {
  return fork(function*(): Saga {
    while (true) {
      const action: Action = yield take(pattern);
      yield fork(saga, ...(args.concat(action) as any));
    }
  });
}
