export type AnyFunction = (...args: any[]) => any;

export type Primitive =
  | null
  | undefined
  | string
  | number
  | boolean
  | symbol
  | bigint;

// prettier-ignore
export type DeepReadonly<T> = {
  readonly [K in keyof T]:
    T[K] extends AnyFunction ? T[K] : 
    T[K] extends Primitive ? T[K] :
    DeepReadonly<T[K]>
};
