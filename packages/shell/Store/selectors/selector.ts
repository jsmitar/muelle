import { select } from '../../../shared/saga/effects';
import { SelectEffect } from '../../../shared/saga/private/types';
import { ShellState } from '../types';

function selector<T extends keyof ShellState>(
  record: T
): SelectEffect<ShellState[T]>;

function selector<T extends keyof ShellState, P extends keyof ShellState[T]>(
  record: T,
  prop: P
): SelectEffect<ShellState[T][P]>;

function selector<T extends keyof ShellState, P extends keyof ShellState[T]>(
  record: T,
  prop?: P
) {
  return select((state: ShellState) =>
    prop ? state[record][prop] : state[record]
  );
}

export default selector;
