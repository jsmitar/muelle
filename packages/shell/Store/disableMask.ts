import { commit, delay } from '../../shared/saga/effects';
import { Saga } from '../../shared/saga/private/types';
import { ENABLE_MASK } from './mutationTypes';
import { durationSelect } from './selectors/durationSelect';

export function* disableMask(): Saga {
  yield delay((yield durationSelect) * 1.5);
  yield commit(ENABLE_MASK, true);
}
