import { Saga, Task } from './private/types';
import {
  call,
  delay,
  cancelled,
  fork,
  all,
  put,
  take,
  cancel,
  race,
  join,
} from './effects';
import { run } from './saga';
import { range } from '../functional';

function* t(name: string) {
  try {
    console.time(name);
    yield delay(1000);
    return `${name}:end`;
  } finally {
    if (yield cancelled()) {
      console.info('task:', name, 'cancelled');
    }
    //console.timeEnd(name);
  }
}

function* emitCancel(time: number) {
  yield fork(function*() {
    yield delay(time);
    yield put('cancel');
  });
}

function* recvCancel() {
  yield take('cancel');
  yield cancel();
}

function* test1(): Saga {
  try {
    const task: Task = yield fork(t, 'job-1');
    yield delay(5000);
    task.cancel();
  } finally {
    if (yield cancelled()) {
      console.log('t1 cancelled');
    }
  }
}

//@ts-ignore
function* test2(): Saga {
  try {
    yield call(emitCancel, 1000);
    const [j1, j2, j3] = yield all([
      call(t, 'j1'),
      call(t, 'j2'),
      call(t, 'j3'),
      call(recvCancel),
    ]);

    console.info('results', j1, j2, j3);
  } finally {
    if (yield cancelled()) {
      console.info('test2 cancelled');
    }
  }
}

//@ts-ignore
function* test3(): Saga {
  const [j1, j2, j3] = yield race([
    call(t, 'j1'),
    call(t, 'j2'),
    call(t, 'j3'),
  ]);

  console.info(j1, j2, j3);
}

function* test4(): Saga {
  try {
    console.time('test4_forks');
    const tasks: Task[] = [];
    for (let i of range(0, 10)) {
      const task: Task = yield fork(function*() {
        try {
          yield delay(100);
          return `fork-${i}`;
        } finally {
          if (yield cancelled()) {
            console.log('fork cancelled:', i);
          }
        }
      });
      tasks.push(task);
    }
    yield delay(50);
    yield cancel();

    const results = yield all(
      tasks.map(task => {
        return call(function*() {
          return yield join(task);
        });
      })
    );
    console.info(...results);
    console.timeEnd('test4_forks');

    console.time('test4_calls');
    const results2 = [];

    for (let i of range(0, 2)) {
      const result = yield call(function*() {
        try {
          yield delay(100);
          return `fork-${i}`;
        } finally {
          if (yield cancelled()) {
            console.log('fork cancelled:', i);
          }
        }
      });
      results2.push(result);
    }
    console.info(...results2);

    console.timeEnd('test4_calls');
  } finally {
    if (yield cancelled()) {
      console.info('test4 cancelled');
    }
  }
}

export function test() {
  [test1, test2, test3, test4].forEach((t, i) => {
    console.log(t.name, 'start');
    i == 3 && run(t);
  });
}
