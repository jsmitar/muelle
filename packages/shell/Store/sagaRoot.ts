import Qt from 'qt';
import {
  takeLatest,
  put,
  call,
  take,
  cancelled,
  delay,
} from '../../shared/saga-tiny/effects';
import { Saga } from '../../shared/saga-tiny/private/types';
import { Action } from '../../shared/flux/flux';
import {
  SLIDE_STATUS,
  ShellState,
  UPDATE_TASK_COUNT_1,
  HIDE_PANEL,
  SHOW_PANEL,
} from './types';

const defaultAction: Action = <any>undefined;

export function* sagaRoot(): Saga {
  yield takeLatest('slide', slide, defaultAction);
}

//@ts-ignore
function animationDuration(state: ShellState) {
  return state.animation.duration;
}

function* slide(action: Action<'in' | 'out'>): Saga {
  try {
    yield call(
      {
        in: slideIn,
        out: slideOut,
      }[action.payload]
    );
  } finally {
    if (yield cancelled()) {
      yield put(SLIDE_STATUS, 'slide-stopped');
    }
  }
}

function* slideOut() {
  yield put(SLIDE_STATUS, 'slide-start-out');
  yield take('slide-finished-out');
  yield put(SLIDE_STATUS, 'slide-stopped');
  yield put(HIDE_PANEL);

  yield put(UPDATE_TASK_COUNT_1);
  Qt.$view.setOpacity(0.01);
  Qt.$positioner.update(0);
}

function* slideIn() {
  yield put(SLIDE_STATUS, 'slide-start-in');
  yield delay(10);
  Qt.$view.setOpacity(1);

  yield take('slide-stopped');
  yield put(SHOW_PANEL);
  yield put(SLIDE_STATUS, 'slide-stopped');
}
