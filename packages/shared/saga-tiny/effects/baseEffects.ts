import {
  effectType,
  CALL,
  TAKE,
  PUT,
  DELAYED,
  FORK,
  CANCEL,
  CANCELLED,
  RACE,
  ALL,
} from '../private/symbols';
import {
  CallEffect,
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
} from '../private/types';

export function call<Fn extends SagaFn>(
  saga: Fn,
  ...args: Parameters<Fn>
): CallEffect {
  return { [effectType]: CALL, saga, args };
}

export function take(pattern: string): TakeEffect {
  return { [effectType]: TAKE, action: { pattern } };
}

export function put(action: { type: string; payload: any }): PutEffect {
  return { [effectType]: PUT, action };
}

export function delayed(delayed: DelayedEffect['delayed']): DelayedEffect {
  return { [effectType]: DELAYED, delayed };
}

export function fork<Fn extends SagaFn>(
  saga: Fn,
  ...args: Parameters<Fn>
): ForkEffect {
  return { [effectType]: FORK, saga, args };
}

export function join(task: ITask) {
  return { type: 'join', task };
}

export function cancel(task?: ITask): CancelEffect {
  return { [effectType]: CANCEL, task };
}

export function cancelled(): CancelledEffect {
  return { [effectType]: CANCELLED };
}

export function race(effects: CallEffect[]): RaceEffect {
  return { [effectType]: RACE, effects, count: effects.length };
}

export function all(effects: CallEffect[]): AllEffect {
  return { [effectType]: ALL, effects, count: effects.length };
}
