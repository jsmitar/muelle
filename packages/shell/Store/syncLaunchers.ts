import { TasksModel } from 'plasma/org.kde.plasma.private.taskmanager-0.1';
import { commit, delay } from '../../shared/saga/effects';
import { Saga } from '../../shared/saga/private/types';
import { ADD_TRANSITION } from './mutationTypes';
import { tasksModelSelect } from './selectors/tasksModelSelect';

export function* syncLaunchers(): Saga {
  yield commit(ADD_TRANSITION, false);

  const model: TasksModel = yield tasksModelSelect;
  model.syncLaunchers();
  yield delay(2000);

  yield commit(ADD_TRANSITION, true);
}
