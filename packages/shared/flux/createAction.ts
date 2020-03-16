import { Action } from './flux';

export type PrepareAction<P, M> = (
  payload?: P,
  meta?: M
) => { payload: P } | { meta: M } | { payload: P; meta: M };

type ActionType = { type: string };

export type AnyActionCreator = ActionType &
  ((payload?: any, meta?: any) => { type: string; payload?: any; meta?: any });

export type ActionCreator<P = any, M = any> = ActionType &
  ((payload: P, meta: M) => Action<P, M>);

export type ActionCreatorDefaultPayload<P = void> = ActionType &
  ((payload?: P) => Action<P, void>);

export type ActionCreatorPrepared<
  PA extends PrepareAction<any, any>
> = ActionType & PA;

export function createAction<P = void, M = void, T extends string = string>(
  type: T
): ActionCreator<P, M>;

export function createAction<
  PA extends PrepareAction<any, any>,
  T extends string = string
>(type: T, prepareAction: PA): ActionCreatorPrepared<PA>;

export function createAction<P = void, T extends string = string>(
  type: T,
  defaultPayload: P
): ActionCreatorDefaultPayload<P>;

export function createAction(type: string, prepareAction?: any): any {
  let actionCreator: any;
  if (prepareAction) {
    if (typeof prepareAction === 'function') {
      actionCreator = (payload: any, meta: any) => {
        return {
          type,
          payload: prepareAction(payload, meta),
        };
      };
    } else {
      actionCreator = (payload: any) => {
        return {
          type,
          payload: payload ?? prepareAction,
        };
      };
    }
  } else {
    actionCreator = (payload: any, meta: any) => {
      return {
        type,
        ...(payload ? { payload } : {}),
        ...(meta ? { meta } : {}),
      };
    };
  }

  actionCreator.type = type;
  return actionCreator;
}
