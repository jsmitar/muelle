import { AnyActionCreator } from 'qrc:/shared/flux/createAction';
import { ActionAny } from 'qrc:/shared/flux/flux';
import { isString } from 'qrc:/shared/functional';
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
} from 'qrc:/shared/saga/private/symbols';
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
} from 'qrc:/shared/saga/private/types';

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

export function take(type: string | AnyActionCreator): TakeEffect {
  const pattern = typeof type === 'string' ? type : type.type;
  return { [effectType]: TAKE, pattern };
}

export function put(action: ActionAny): PutEffect;
export function put(type: string, payload?: any): PutEffect;

export function put(action: any, payload?: any): PutEffect {
  if (isString(action)) {
    return { [effectType]: PUT, type: action, payload };
  }
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
