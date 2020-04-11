import Muelle from '@qml/org.muelle.types-1.0';
import { Action } from '../../../shared/flux/flux';
import {
  cancelled,
  commit,
  delay,
  put,
  take,
} from '../../../shared/saga/effects';
import { slideIn, slideOut, slideOutFinished } from '../actions';
import {
  NEXT_EDGE,
  UPDATE_POSITION,
  UPDATING_ORIENTATION,
} from '../mutationTypes';
import { contextSelect } from '../selectors/contextSelect';
import { Context } from '../types';

export function* edge(action: Action<Muelle.Types.Edge>) {
  const { positioner, view, configuration }: Context = yield contextSelect;
  try {
    // slide out
    yield put(slideOut());
    yield take(slideOutFinished);
    yield commit(UPDATING_ORIENTATION, action.payload);

    // prepare for slide in
    yield commit(NEXT_EDGE, action.payload);
    configuration.edge = action.payload;
    configuration.save();
    positioner.update(0);
    yield commit(UPDATE_POSITION);
    yield delay(200);

    // slide in
    yield put(slideIn());
  } finally {
    if (yield cancelled()) {
      view.setOpacity(1);
      yield put(slideIn());
    }
    yield commit(UPDATING_ORIENTATION, action.payload);
  }
}
