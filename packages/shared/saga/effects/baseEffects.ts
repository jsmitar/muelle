import {
  ALL,
  CALL,
  CANCEL,
  CANCELLED,
  DELAYED,
  effectType,
  FORK,
  JOIN,
  PUT,
  RACE,
  SELECT,
  TAKE,
} from '../private/symbols';
import {
  AllEffect,
  CallEffect,
  CancelEffect,
  CancelledEffect,
  DelayedEffect,
  ForkEffect,
  JoinEffect,
  PutEffect,
  RaceEffect,
  SagaFn,
  SelectEffect,
  TakeEffect,
  Task,
} from '../private/types';

export function call<Fn extends SagaFn>(
  saga: Fn,
  ...args: Parameters<Fn>
): CallEffect {
  return { [effectType]: CALL, name: saga.name, saga, args };
}

export function select<S = any, R = any>(
  selector: (state: S) => R
): SelectEffect<R> {
  return { [effectType]: SELECT, selector };
}

export function take(pattern: string): TakeEffect {
  return { [effectType]: TAKE, pattern };
}

export function put(type: string, ...args: any[]): PutEffect {
  return { [effectType]: PUT, type, args };
}

export function delayed(delayed: DelayedEffect['delayed']): DelayedEffect {
  return { [effectType]: DELAYED, delayed };
}

export function fork<Fn extends SagaFn>(
  saga: Fn,
  ...args: Parameters<Fn>
): ForkEffect {
  return { [effectType]: FORK, name: saga.name, saga, args };
}

export function forkNamed<Fn extends SagaFn>(
  saga: Fn,
  name: string,
  ...args: Parameters<Fn>
): ForkEffect {
  return { [effectType]: FORK, name, saga, args };
}

export function join(task: Task): JoinEffect {
  return { [effectType]: JOIN, task };
}

export function cancel(task?: Task): CancelEffect {
  return task ? { [effectType]: CANCEL, task } : { [effectType]: CANCEL };
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
