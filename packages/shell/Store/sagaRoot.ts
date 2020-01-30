import { takeLatest } from '../../shared/saga/effects';
import { Saga } from '../../shared/saga/private/types';
import { edge } from './sagas/edge';
import { slide } from './sagas/slide';
import { updateTaskCount1 } from './sagas/updateTaskCount1';

export function* sagaRoot(): Saga {
  yield takeLatest('updateTaskCount1', updateTaskCount1);
  yield takeLatest('slide', slide);
  yield takeLatest('changeEdge', edge);
}
