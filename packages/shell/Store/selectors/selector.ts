import { select } from '../../../shared/saga/effects';
import { SelectEffect } from '../../../shared/saga/private/types';
import { ShellStateReadonly } from '../types';

function selector<S extends ShellStateReadonly, K extends keyof S>(
  record: keyof S
): SelectEffect<S[K]>;

function selector<
  S extends ShellStateReadonly,
  K1 extends keyof S,
  K2 extends keyof S[K1]
>(record: K1, prop: K2): SelectEffect<S[K1][K2]>;

function selector<
  S extends ShellStateReadonly,
  K1 extends keyof S,
  K2 extends keyof S[K1]
>(record: K1, prop?: K2) {
  return select<S, S[K1] | S[K1][K2]>(state =>
    prop ? state[record][prop] : state[record]
  );
}

export default selector;
