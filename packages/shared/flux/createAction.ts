import { Action } from './flux';

export type PrepareAction<P, M> =
  | ((payload: P, meta: M) => { payload: P })
  | ((payload: P, meta: M) => { meta: M })
  | ((payload: P, meta: M) => { payload: P; meta: M });

type ActionType = { type: string };

export type ActionCreator<P = void, M = void> = ActionType &
  ((payload: P, meta: M) => Action<P, M>);

export function createAction<P = void, M = void, T extends string = string>(
  type: T,
  prepareAction?: PrepareAction<P, M>
): ActionCreator<P, M>;

export function createAction(type: string, prepareAction?: Function): any {
  if (prepareAction) {
    const actionCreator: any = (payload: any, meta: any) => {
      return {
        type,
        ...prepareAction(payload, meta),
      };
    };
    actionCreator.type = type;

    return actionCreator;
  }

  const actionCreator: any = (payload: any) => {
    return payload ? { type, payload } : { type };
  };
  actionCreator.type = type;

  return actionCreator;
}
