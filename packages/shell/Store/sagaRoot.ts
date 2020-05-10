import {
  cancel,
  delayed,
  fork,
  takeLatest,
  takeLeading,
} from '../../shared/saga/effects';
import { Saga, Task } from '../../shared/saga/private/types';
import * as action from './actions';
import { disableMask } from './disableMask';
import { changeScreen } from './sagas/changeScreen';
import { edge } from './sagas/edge';
import { slide } from './sagas/slide';
import { updateTaskCount1 } from './sagas/updateTaskCount1';
import { contextSelect } from './selectors/contextSelect';
import { syncLaunchers } from './syncLaunchers';
import { Context } from './types';

export function* sagaRoot(): Saga {
  const task: Task = yield fork(function* (): Saga {
    yield takeLatest(action.updateTaskCount1, updateTaskCount1);
    yield takeLatest(action.slide, slide);
    yield takeLatest(action.changeEdge, edge);
    yield takeLatest(action.disableMask, disableMask);
    yield takeLatest(action.syncLaunchers, syncLaunchers);
    yield takeLeading(action.changeScreen, changeScreen);
  });

  const { view }: Context = yield contextSelect;
  yield delayed(resolve => {
    view.release.connect(resolve);
    return () => view.release.disconnect(resolve);
  });

  yield cancel(task);
  console.info('SAGA ROOT STOPPPED');
}
