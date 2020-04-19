import Qt from 'qt';
import { L } from 'ts-toolbelt';
import { AnyActionCreator } from '../../flux/createAction';
import { ActionAny } from '../../flux/flux';
import { TaskStatus } from '../private/symbols';
import { Saga, SagaFn, Task } from '../private/types';
import { call, cancel, delayed, fork, take } from './baseEffects';

export function delay(ms: number) {
  return delayed(resolve => {
    const timer = Qt.setTimeout(resolve, ms);
    return () => Qt.clearTimeout(timer);
  });
}

// @ts-ignore
export function takeLeading<Fn extends SagaFn>(
  pattern: string,
  saga: Fn,
  ...args: L.Pop<Parameters<Fn>>
) {
  const name = saga.name;

  return fork(function* takeLeading(): Saga {
    while (true) {
      const action: ActionAny = yield take(pattern);
      const effect = call<SagaFn>(saga, ...args, action);
      effect.name = name;
      yield effect;
    }
  });
}

export function takeLatest<Fn extends SagaFn>(
  pattern: string | AnyActionCreator,
  saga: Fn,
  ...args: L.Pop<Parameters<Fn>>
) {
  const name = saga.name;

  return fork(function* takeLatest(): Saga {
    let lastTask: Task | undefined;

    while (true) {
      const action: ActionAny = yield take(pattern);
      if (lastTask && lastTask.status === TaskStatus.Running)
        yield cancel(lastTask);
      const effect = fork<SagaFn>(saga, ...args, action);
      effect.name = name;
      yield effect;
    }
  });
}

export function takeEvery<Fn extends SagaFn>(
  pattern: string,
  saga: Fn,
  ...args: L.Pop<Parameters<Fn>>
) {
  return fork(function* takeEvery(): Saga {
    while (true) {
      const action: ActionAny = yield take(pattern);
      yield fork<SagaFn>(saga, ...args, action);
    }
  });
}
