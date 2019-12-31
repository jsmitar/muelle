import {
  effect,
  CALL,
  TAKE,
  PUT,
  DELAYED,
  FORK,
  CANCEL,
  CANCELLED,
  RACE,
  ALL,
} from './symbols';
import {
  CallEffect,
  Callback,
  DelayedEffect,
  ForkEffect,
  RaceEffect,
  AllEffect,
  TakeEffect,
  CancelledEffect,
  ITask,
  CancelEffect,
  PutEffect,
  SagaFn,
} from '../types';

export function call<Fn extends (...args: any) => any>(
  fn: Fn,
  ...args: Parameters<Fn>
): CallEffect {
  return { [effect]: true, type: CALL, fn, args };
}

export function take(pattern: string): TakeEffect {
  return { [effect]: true, type: TAKE, action: { pattern } };
}

export function put(action: { type: string; payload: any }): PutEffect {
  return { [effect]: true, type: PUT, action };
}

export function delayed<T>(
  fn: (resolve: Callback<T>, reject: Callback) => any
): DelayedEffect {
  return { [effect]: true, type: DELAYED, fn };
}

export function fork<Fn extends SagaFn>(
  saga: Fn,
  ...args: Parameters<Fn>
): ForkEffect {
  return { [effect]: true, type: FORK, saga, args };
}

export function join(task: ITask) {
  return { type: 'join', task };
}

export function cancel(task?: ITask): CancelEffect {
  return { [effect]: true, type: CANCEL, task };
}

export function cancelled(): CancelledEffect {
  return { [effect]: true, type: CANCELLED };
}

export function race(effects: CallEffect[]): RaceEffect {
  return { [effect]: true, type: RACE, effects };
}

export function all(effects: CallEffect[]): AllEffect {
  return { [effect]: true, type: ALL, effects };
}
