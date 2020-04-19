import {
  all,
  call,
  cancel,
  cancelled,
  commit,
  delay,
  fork,
  join,
  put,
  race,
  select,
  take,
} from '../shared/saga/effects';
import EventEmitter from '../shared/saga/eventEmitter';
import { TaskStatus } from '../shared/saga/private/symbols';
import { SagaFn } from '../shared/saga/private/types';
import { runSaga, TaskContext, TaskController } from '../shared/saga/saga';

const mockState = {
  str: 'string value',
  num: 2,
  arr: Array(3).fill(0),
};

type State = typeof mockState;

const emitter = new EventEmitter();

const commitcb = jest.fn(() => {});

const dispatch = jest.fn(action => emitter.emit(action.type, action.payload));

const context: TaskContext = {
  actionSubscriber: emitter,
  commit: commitcb,
  dispatch,
  getState: () => mockState,
  // sagaMonitor: simpleSagaMonitor,
};

const saga = (fn: SagaFn) => {
  return new Promise<[any, TaskStatus]>((resolve, reject) => {
    try {
      <TaskController>runSaga(context, {
        saga: fn,
        name: fn.name,
        onFinish: (...args) => resolve(args),
      });
    } catch (e) {
      reject(e);
    }
  });
};

test('effect call', async () => {
  expect.assertions(2);
  const callback = jest.fn(function* (val: boolean) {
    expect(val).toBeTruthy();
  });

  await saga(function* () {
    yield call(callback, true);
    yield call(callback, true);
  });
});

test('effect select', async () => {
  const [r] = await saga(function* () {
    return yield select((state: State) => state.arr);
  });

  expect(r).toHaveLength(3);
});

test('effect take/put', async () => {
  const [r] = await saga(function* () {
    yield fork(function* () {
      yield put('value', 1);
    });

    return yield take('value');
  });

  expect(r).toBe(1);
});

test('effect commit', async () => {
  await saga(function* () {
    const t = yield fork(function* () {
      yield commit('commit-1', 1, 2, 3);
      yield commit('commit-2', 3, 4, 5);
    });

    return yield join(t);
  });

  expect(commitcb.mock.calls[0]).toEqual(['commit-1', 1, 2, 3]);
  expect(commitcb.mock.calls[1]).toEqual(['commit-2', 3, 4, 5]);
});

test('effect delay', async () => {
  await saga(function* () {
    yield delay(200);
  });
});

test('effect cancel', async () => {
  const cb = jest.fn();

  const r = await saga(function* effect_cancel() {
    try {
      yield delay(200);
      yield cancel();
    } finally {
      if (yield cancelled()) {
        cb();
      }
      return 'done';
    }
  });

  expect(cb).toBeCalledTimes(1);
  expect(r).toEqual(['done', TaskStatus.Cancelled]);
});

test('effect race/delay', async () => {
  const cb = jest.fn((_: number) => {});
  await saga(function* () {
    yield fork(function* () {
      yield delay(100);
      yield put('stop');
    });

    yield race([
      call(function* () {
        yield take('stop');
      }),
      call(function* () {
        try {
          yield delay(5000);
        } finally {
          if (yield cancelled()) {
            cb(1);
            yield put('finish');
          }
        }
      }),
      call(function* () {
        try {
          yield delay(5000);
        } finally {
          if (yield cancelled()) {
            cb(2);
            yield put('finish');
          }
        }
      }),
    ]);

    yield take('finish');
    yield take('finish');
  });

  expect(cb).nthCalledWith(1, 1);
  expect(cb).nthCalledWith(2, 2);
}, 400);

test('effect all', async () => {
  const arr = Array.from({ length: 50 }).map((_, i) => i);

  const r = await saga(function* () {
    return yield all(
      arr.map((_, i) =>
        call(function* () {
          yield delay(10);
          return i;
        })
      )
    );
  });

  expect(r).toEqual([arr, TaskStatus.Done]);
}, 25);

test('cancel propagation', async () => {
  const cb = jest.fn();

  await saga(function* cancel_propagation() {
    yield fork(function* _1() {
      try {
        yield fork(function* _1_1() {
          try {
            yield fork(function* _1_1_1() {
              try {
                yield delay(1000);
              } finally {
                if (yield cancelled()) cb(_1_1_1.name);
              }
            });
            yield delay(1000);
          } finally {
            if (yield cancelled()) cb(_1_1.name);
          }
        });
        yield delay(10);
        cb('cancel');
        yield cancel();
      } finally {
        if (yield cancelled()) cb(_1.name);
      }
    });
  });

  expect(cb).nthCalledWith(1, 'cancel');
  expect(cb).nthCalledWith(2, '_1_1_1');
  expect(cb).nthCalledWith(3, '_1_1');
  expect(cb).nthCalledWith(4, '_1');
});

test('takeEvery', async () => {});
