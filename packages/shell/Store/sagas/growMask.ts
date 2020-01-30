import { commit, delay } from '../../../shared/saga/effects';
import { durationSelect } from '../selectors/durationSelect';
import { MASK_GROWING } from '../types';

export function* growMask() {
  const duration: number = yield durationSelect;
  yield commit(MASK_GROWING, true);
  yield delay(duration / 2);
  yield commit(MASK_GROWING, false);
}
