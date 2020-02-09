import { commit, delay } from '../../../shared/saga/effects';
import { MASK_GROWING } from '../mutationTypes';
import { durationSelect } from '../selectors/durationSelect';

export function* growMask() {
  const duration: number = yield durationSelect;
  yield commit(MASK_GROWING, true);
  yield delay(duration);
  yield commit(MASK_GROWING, false);
}
