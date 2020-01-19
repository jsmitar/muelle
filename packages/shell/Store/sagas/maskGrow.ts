import { put } from '../../../shared/saga/effects';
import { MASK_GROW } from '../types';

export function* maskGrow() {
  yield put(MASK_GROW, true);
}
