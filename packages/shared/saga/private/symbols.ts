export const CALL = 'CALL';
export const SELECT = 'SELECT';
export const TAKE = 'TAKE';
export const PUT = 'PUT';
export const DELAYED = 'DELAYED';
export const FORK = 'FORK';
export const JOIN = 'JOIN';
export const CANCEL = 'CANCEL';
export const CANCELLED = 'CANCELLED';
export const ALL = 'ALL';
export const RACE = 'RACE';

export const FINISH = '@saga/finish';

export const effectType = Symbol('@saga/effect');

export enum TaskStatus {
  Running,
  Done,
  Aborted,
  Cancelled,
}
