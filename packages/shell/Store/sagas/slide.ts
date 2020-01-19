import Qt from 'qt';
import { Action } from '../../../shared/flux/flux';
import { call, cancelled, put, take } from '../../../shared/saga/effects';
import { Saga } from '../../../shared/saga/private/types';
import { defaultAction } from '../sagaRoot';
import {
  HIDE_PANEL,
  SHOW_PANEL,
  SLIDE_STATUS,
  UPDATE_TASK_COUNT_1,
} from '../types';

export function* slide(action: Action<'in' | 'out'> = defaultAction): Saga {
  try {
    action.payload === 'in' ? yield call(slideIn) : yield call(slideOut);
  } finally {
    if (yield cancelled()) {
      yield put(SLIDE_STATUS, 'none');
    }
  }
}

export function* slideOut() {
  yield put(SLIDE_STATUS, 'slide-out-running');
  yield take('slide-out-finished');
  yield put(SLIDE_STATUS, 'none');
  yield put(HIDE_PANEL);

  yield put(UPDATE_TASK_COUNT_1);
  Qt.$view.setOpacity(0.01);
  Qt.$positioner.update(0);
}

export function* slideIn() {
  Qt.$view.setOpacity(1);

  yield put(SHOW_PANEL);
  yield put(SLIDE_STATUS, 'slide-in-running');
  yield take('slide-in-finished');
  yield put(SLIDE_STATUS, 'none');
}
