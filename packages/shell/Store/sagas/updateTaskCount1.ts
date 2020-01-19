import { Action } from '../../../shared/flux/flux';
import { put, select, take } from '../../../shared/saga/effects';
import { Saga } from '../../../shared/saga/private/types';
import { defaultAction } from '../sagaRoot';
import panelSelector from '../selectors/panelSelector';
import {
  MASK_GROW,
  NEXT_TASK_COUNT1,
  Panel,
  UPDATE_TASK_COUNT_1,
} from '../types';

export function* updateTaskCount1(
  action: Action<number> = defaultAction
): Saga {
  yield put(NEXT_TASK_COUNT1, action.payload);
  const panel: Panel = yield select(panelSelector);

  if (action.payload > panel.taskCount1) {
    yield put(UPDATE_TASK_COUNT_1);
    yield put(MASK_GROW, true);
  } else if (action.payload < panel.taskCount1) {
    yield take('slide-out-finished');
    yield put(UPDATE_TASK_COUNT_1);
  }
}
