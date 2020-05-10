import { Action } from '../../../shared/flux/flux';
import { commit, delay, put, take } from '../../../shared/saga/effects';
import { Saga } from '../../../shared/saga/private/types';
import {
  slideIn,
  slideInFinished,
  slideOut,
  slideOutFinished,
} from '../actions';
import { UPDATING_SCREEN } from '../mutationTypes';
import { contextSelect } from '../selectors/contextSelect';
import { Context } from '../types';

export function* changeScreen(action: Action<string>): Saga {
  const {
    positioner,
    view,
    container,
    configuration,
  }: Context = yield contextSelect;

  const screenName = action.payload;

  // slide out
  yield commit(UPDATING_SCREEN, true);
  yield put(slideOut());
  yield take(slideOutFinished);

  // prepare for slide in
  if (container.screens[screenName]) {
    configuration.screen = screenName;
    configuration.save();
    view.screen = container.screens[screenName];
    positioner.update(0);
  }
  yield delay(200);

  // slide in
  yield put(slideIn());
  yield take(slideInFinished);
  yield commit(UPDATING_SCREEN, false);
}
