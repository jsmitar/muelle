import { ActionCreator } from '../../flux/createAction';
import { ActionAny } from '../../flux/flux';
import {
  ALL,
  CALL,
  CANCEL,
  CANCELLED,
  COMMIT,
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
  CommitEffect,
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

export function take(type: string | ActionCreator): TakeEffect {
  const pattern = typeof type === 'string' ? type : type.type;
  return { [effectType]: TAKE, pattern };
}

export function put(action: ActionAny): PutEffect {
  return { [effectType]: PUT, ...action };
}

export function commit(type: string, ...args: any[]): CommitEffect {
  return { [effectType]: COMMIT, type, args };
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
