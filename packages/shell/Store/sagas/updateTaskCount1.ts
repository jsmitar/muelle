import { Action } from '../../../shared/flux/flux';
import { call, cancelled, put } from '../../../shared/saga/effects';
import { Saga } from '../../../shared/saga/private/types';
import { defaultAction } from '../sagaRoot';
import { taskCount1Select } from '../selectors/taskCount1Select';
import { MASK_GROWING, NEXT_TASK_COUNT1, UPDATE_TASK_COUNT_1 } from '../types';
import { growMask } from './growMask';

export function* updateTaskCount1(
  action: Action<number> = defaultAction
): Saga {
  try {
    const nextTaskCount = action.payload;
    yield put(NEXT_TASK_COUNT1, nextTaskCount);

    if (nextTaskCount > (yield taskCount1Select)) {
      yield put(UPDATE_TASK_COUNT_1);
      yield call(growMask);
    } else {
      yield call(growMask);
      yield put(UPDATE_TASK_COUNT_1);
    }
  } finally {
    if (yield cancelled()) {
      yield put(UPDATE_TASK_COUNT_1);
      yield put(MASK_GROWING, false);
    }
  }
}
