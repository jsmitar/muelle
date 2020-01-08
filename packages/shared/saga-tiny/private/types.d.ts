import { effectType, TaskStatus } from './symbols';

export interface Saga extends Generator<Effect, any, any> {}

export interface SagaFn {
  (...args: any[]): Saga;
}

export interface Task<T = any> {
  taskId: string;
  result: T;
  status: TaskStatus;
  cancel(): void;
}

export type EffectType =
  | 'CALL'
  | 'TAKE'
  | 'PUT'
  | 'DELAYED'
  | 'FORK'
  | 'JOIN'
  | 'CANCEL'
  | 'CANCELLED'
  | 'ALL'
  | 'RACE';

// prettier-ignore
export type Effect<T extends EffectType = EffectType> = 
  T extends 'CALL' ? CallEffect :
  T extends 'TAKE' ? TakeEffect :
  T extends 'PUT' ? PutEffect :
  T extends 'DELAYED' ? DelayedEffect :
  T extends 'FORK' ? ForkEffect :
  T extends 'JOIN' ? JoinEffect :
  T extends 'CANCEL' ? CancelEffect :
  T extends 'CANCELLED' ? CancelledEffect :
  T extends 'ALL' ? AllEffect :
  T extends 'RACE' ? RaceEffect : 
  EffectBase

export interface EffectBase<T extends EffectType = EffectType> {
  [effectType]: T;
}

export interface CallEffect<Fn extends SagaFn = SagaFn>
  extends EffectBase<'CALL'> {
  saga: SagaFn;
  args: Parameters<Fn>;
}

export interface TakeEffect extends EffectBase<'TAKE'> {
  action: { pattern: string };
}

export interface PutEffect extends EffectBase<'PUT'> {
  action: { type: string; payload?: any };
}

export type ClearFunction = () => any;

export interface DelayedEffect extends EffectBase<'DELAYED'> {
  delayed: (resolve: (result?: any) => void) => ClearFunction;
}

export interface ForkEffect extends EffectBase<'FORK'> {
  saga: SagaFn;
  args: any[];
}

export interface JoinEffect extends EffectBase<'JOIN'> {
  task: Task;
}

export interface CancelEffect extends EffectBase<'CANCEL'> {
  task?: Task;
}

export interface CancelledEffect extends EffectBase<'CANCELLED'> {}

export interface AllEffect extends EffectBase<'ALL'> {
  effects: CallEffect[];
  count: number;
}

export interface RaceEffect extends EffectBase<'RACE'> {
  effects: CallEffect[];
  count: number;
}
