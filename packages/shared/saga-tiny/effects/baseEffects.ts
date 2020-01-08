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
  JOIN,
} from '../private/symbols';
import {
  CallEffect,
  DelayedEffect,
  ForkEffect,
  RaceEffect,
  AllEffect,
  TakeEffect,
  CancelledEffect,
  Task,
  CancelEffect,
  PutEffect,
  SagaFn,
  JoinEffect,
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

export function put(type: string, payload?: any): PutEffect {
  return { [effectType]: PUT, action: { type, payload } };
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

export function join(task: Task): JoinEffect {
  return { [effectType]: JOIN, task };
}

export function cancel(task?: Task): CancelEffect {
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
