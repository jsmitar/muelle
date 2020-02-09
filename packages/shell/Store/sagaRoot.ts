import { takeLatest } from '../../shared/saga/effects';
import { Saga } from '../../shared/saga/private/types';
import * as action from './actions';
import { disableMask } from './disableMask';
import { edge } from './sagas/edge';
import { slide } from './sagas/slide';
import { updateTaskCount1 } from './sagas/updateTaskCount1';

export function* sagaRoot(): Saga {
  yield takeLatest(action.updateTaskCount1, updateTaskCount1);
  yield takeLatest(action.slide, slide);
  yield takeLatest(action.changeEdge, edge);
  yield takeLatest(action.disableMask, disableMask);
}
