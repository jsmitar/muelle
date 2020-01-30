import Muelle from '@qml/org.muelle.types-1.0';
import { Action } from '../../../shared/flux/flux';
import { put, take } from '../../../shared/saga/effects';
import { slideInFinished, slideOut } from '../actions';

export function* edge(_action: Action<Muelle.Types.Edge>) {
  yield put(slideOut());
  yield take(slideInFinished);
}
