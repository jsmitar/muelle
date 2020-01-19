import { Action } from '../../shared/flux/flux';
import { takeLatest } from '../../shared/saga/effects';
import { Saga } from '../../shared/saga/private/types';
import { slide } from './sagas/slide';
import { updateTaskCount1 } from './sagas/updateTaskCount1';

export const defaultAction: Action = <any>undefined;

export function* sagaRoot(): Saga {
  yield takeLatest('updateTaskCount1', updateTaskCount1);
  yield takeLatest('slide', slide);
}
