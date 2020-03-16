import { Action } from '../../../shared/flux/flux';
import { call, cancelled, commit } from '../../../shared/saga/effects';
import { Saga } from '../../../shared/saga/private/types';
import {
  MASK_GROWING,
  NEXT_TASK_COUNT1,
  SEPARATOR_COUNT,
  UPDATE_TASK_COUNT_1,
} from '../mutationTypes';
import { taskCount1Select } from '../selectors/taskCount1Select';
import { growMask } from './growMask';
import { separatorCountSelect } from './separatorCount';

export function* updateTaskCount1(
  action: Action<{ tasks: number; separators: number }>
): Saga {
  try {
    const { tasks, separators } = action.payload;
    yield commit(NEXT_TASK_COUNT1, tasks);

    if (separators !== (yield separatorCountSelect)) {
      yield commit(SEPARATOR_COUNT, separators);
    }

    if (tasks > (yield taskCount1Select)) {
      yield commit(UPDATE_TASK_COUNT_1);
      yield call(growMask);
    } else {
      yield call(growMask);
      yield commit(UPDATE_TASK_COUNT_1);
    }
  } finally {
    if (yield cancelled()) {
      yield commit(UPDATE_TASK_COUNT_1);
      yield commit(MASK_GROWING, false);
    }
  }
}
