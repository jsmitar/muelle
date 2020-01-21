import { Action } from '../../../shared/flux/flux';
import { delay, put } from '../../../shared/saga/effects';
import { Saga } from '../../../shared/saga/private/types';
import { defaultAction } from '../sagaRoot';
import selector from '../selectors/selector';
import { NEXT_TASK_COUNT1, UPDATE_TASK_COUNT_1 } from '../types';

const taskCount1Select = selector('panel', 'taskCount1');
const durationSelect = selector('animation', 'duration');

export function* updateTaskCount1(
  action: Action<number> = defaultAction
): Saga {
  yield put(NEXT_TASK_COUNT1, action.payload);
  const taskCount1: number = yield taskCount1Select;

  if (action.payload >= taskCount1) {
    yield put(UPDATE_TASK_COUNT_1);
  } else if (action.payload < taskCount1) {
    const duration: number = yield durationSelect;
    yield delay(duration);
    yield put(UPDATE_TASK_COUNT_1);
  }
}
