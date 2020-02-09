import { commit, delay, takeLatest } from '../../shared/saga/effects';
import { Saga } from '../../shared/saga/private/types';
import * as action from './actions';
import { ENABLE_MASK } from './mutationTypes';
import { edge } from './sagas/edge';
import { slide } from './sagas/slide';
import { updateTaskCount1 } from './sagas/updateTaskCount1';
import { durationSelect } from './selectors/durationSelect';

export function* sagaRoot(): Saga {
  yield takeLatest(action.updateTaskCount1, updateTaskCount1);
  yield takeLatest(action.slide, slide);
  yield takeLatest(action.changeEdge, edge);
  yield takeLatest(action.disableMask, disableMask);
}

export function* disableMask(): Saga {
  yield delay((yield durationSelect) * 1.5);
  yield commit(ENABLE_MASK, true);
}
