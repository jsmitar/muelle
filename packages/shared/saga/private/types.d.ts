import { effectType, TaskStatus } from 'qrc:/shared/saga/private/symbols';

export interface Saga extends Generator<Effect, any, any> {}

// prettier-ignore
export type Effect<T extends EffectType = EffectType> = 
  T extends 'CALL' ? CallEffect :
  T extends 'SELECT' ? SelectEffect :
  T extends 'TAKE' ? TakeEffect :
  T extends 'PUT' ? PutEffect :
  T extends 'COMMIT' ? CommitEffect :
  T extends 'DELAYED' ? DelayedEffect :
  T extends 'FORK' ? ForkEffect :
  T extends 'JOIN' ? JoinEffect :
  T extends 'CANCEL' ? CancelEffect : 
  T extends 'CANCELLED' ? CancelledEffect :
  T extends 'ALL' ? AllEffect :
  T extends 'RACE' ? RaceEffect : 
  never

export interface SagaFn {
  (...args: any[]): Saga;
}

export interface Task<T = any> {
  taskId: string;
  result: T;
  status: TaskStatus;
  name: string;
}

export type EffectType =
  | 'CALL'
  | 'SELECT'
  | 'TAKE'
  | 'PUT'
  | 'COMMIT'
  | 'DELAYED'
  | 'FORK'
  | 'JOIN'
  | 'CANCEL'
  | 'CANCELLED'
  | 'ALL'
  | 'RACE';

export interface EffectBase<T extends EffectType = EffectType> {
  [effectType]: T;
}

export interface CallEffect<Fn extends SagaFn = SagaFn>
  extends EffectBase<'CALL'> {
  saga: SagaFn;
  name: string;
  args: Parameters<Fn>;
}

export interface SelectEffect<T = any> extends EffectBase<'SELECT'> {
  selector: (state: any) => T;
}

export interface TakeEffect extends EffectBase<'TAKE'> {
  pattern: string;
}

export interface PutEffect extends EffectBase<'PUT'> {
  type: string;
  payload?: any;
  meta?: any;
}

export interface CommitEffect extends EffectBase<'COMMIT'> {
  type: string;
  args: any[];
}

export type ClearFunction = () => any;

export interface DelayedEffect extends EffectBase<'DELAYED'> {
  delayed: (resolve: (result?: any) => void) => ClearFunction;
}

export interface ForkEffect extends EffectBase<'FORK'> {
  saga: SagaFn;
  name: string;
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
