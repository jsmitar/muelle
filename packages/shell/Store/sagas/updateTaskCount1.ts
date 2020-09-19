import { durationSelect } from 'qrc:/shell/Store/selectors/durationSelect';
import { Action } from '../../../shared/flux/flux';
import { commit, delay } from '../../../shared/saga/effects';
import { Saga } from '../../../shared/saga/private/types';
import {
  MASK_GROWING,
  NEXT_TASK_COUNT1,
  SEPARATOR_COUNT,
  UPDATE_TASK_COUNT_1,
} from '../mutationTypes';
import { taskCount1Select } from '../selectors/taskCount1Select';
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

    const currentTasks: number = yield taskCount1Select;
    const duration: number = yield durationSelect;

    if (tasks > currentTasks) {
      // panel width growing
      yield commit(MASK_GROWING, true);
      yield commit(UPDATE_TASK_COUNT_1);
      yield delay(duration);
    } else {
      // panel width shrinking
      yield commit(MASK_GROWING, true);
      yield delay(duration);
      yield commit(UPDATE_TASK_COUNT_1);
    }
  } finally {
    yield commit(MASK_GROWING, false);
  }
}
