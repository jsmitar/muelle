export const CALL = 'CALL';
export const TAKE = 'TAKE';
export const PUT = 'PUT';
export const DELAYED = 'DELAYED';
export const FORK = 'FORK';
export const JOIN = 'JOIN';
export const CANCEL = 'CANCEL';
export const CANCELLED = 'CANCELLED';
export const ALL = 'ALL';
export const RACE = 'RACE';
export const effect = Symbol('@saga-tiny/effect');

export type EffectSymbol = typeof effect;

export enum TaskStatus {
  Running,
  Done,
  Aborted,
  Cancelled,
}

export const m_private = Symbol('@saga-tiny/private');
