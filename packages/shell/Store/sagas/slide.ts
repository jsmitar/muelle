import { Action } from '../../../shared/flux/flux';
import { call, commit, take } from '../../../shared/saga/effects';
import { Saga } from '../../../shared/saga/private/types';
import {
  HIDE_PANEL,
  SHOW_PANEL,
  SLIDE_STATUS,
  UPDATE_TASK_COUNT_1,
} from '../mutationTypes';
import { contextSelect } from '../selectors/contextSelect';
import { Context } from '../types';

export function* slide(action: Action<'in' | 'out'>): Saga {
  try {
    action.payload === 'in' ? yield call(slideIn) : yield call(slideOut);
  } finally {
    // if (yield cancelled()) {
    //   yield commit(SLIDE_STATUS, 'none');
    // }
  }
}

export function* slideOut() {
  yield commit(SLIDE_STATUS, 'slide-out-running');
  yield take('slide-out-finished');
  yield commit(SLIDE_STATUS, 'none');
  yield commit(HIDE_PANEL);

  yield commit(UPDATE_TASK_COUNT_1);
  const context: Context = yield contextSelect;

  context.view.setOpacity(0.01);
  context.positioner.update(0);
}

export function* slideIn() {
  const context: Context = yield contextSelect;
  context.view.setOpacity(1);

  yield commit(SHOW_PANEL);
  yield commit(SLIDE_STATUS, 'slide-in-running');
  yield take('slide-in-finished');
  yield commit(SLIDE_STATUS, 'none');
}
