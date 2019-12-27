import { effect, EffectSymbol, TaskStatus } from 'private/symbols';

export type Callback<Args = any, R = any> = (...args: Args[]) => R;

export type Saga = Generator<any, any, EffectType>;

export type SagaFn = (...args: any[]) => Saga;

export interface ITask<T = any> {
  taskId: string;
  result: T;
  status: TaskStatus;
}

export interface ITaskPrivate extends ITask {
  start(): void;
  abort(e: Error): void;
  cancel(): void;
  deleteSubTask(taskId: number): void;
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

export type EffectPayload<T extends Effect<any>> = Omit<
  T,
  EffectSymbol | 'type'
>;

export type Effect<T extends EffectType = EffectType, P = any> = {
  [effect]: true;
  type: T;
} & P;

export type CallEffect = Effect<'CALL', { fn: Callback; args: any[] }>;

export type TakeEffect = Effect<'TAKE', { action: { pattern: string } }>;

export type PutEffect = Effect<
  'PUT',
  { action: { type: string; payload: any } }
>;

export type DelayedEffect = Effect<
  'DELAYED',
  { fn: (resolve: Callback<any>, reject: Callback) => any }
>;

export type ForkEffect = Effect<'FORK', { saga: SagaFn; args: any[] }>;

export type JoinEffect = Effect<'JOIN', { task: ITask }>;

export type CancelEffect = Effect<'CANCEL', { task?: ITask }>;

export type CancelledEffect = Effect<'CANCELLED'>;

export type AllEffect = Effect<'ALL', { effects: CallEffect[] }>;

export type RaceEffect = Effect<'RACE', { effects: CallEffect[] }>;
