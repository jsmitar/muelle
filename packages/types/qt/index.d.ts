export = Qt;
export as namespace Qt;

declare namespace Qt {
  export const Promise: PromiseConstructor;

  function callLater<Fn extends (...args: any[]) => any>(
    callback: Fn,
    ...args: Parameters<Fn>
  ): void;
  function setTimeout<Fn extends (...args: any[]) => any>(
    handler: Fn,
    ms: number
  ): number;
  function clearTimeout(handle: number): void;

  function rect(x: number, y: number, width: number, height: number): rect;
  function size(width: number, height: number): size;
  function point(x: number, y: number): point;

  function rgba(red: number, green: number, blue: number, alpha: number): color;

  export type rect = size & point;

  export type size = {
    width: number;
    height: number;
  };

  export type point = {
    x: number;
    y: number;
  };

  export type color = {
    r: number;
    g: number;
    b: number;
  };

  export interface Signal {
    connect(slot: (...args: any[]) => any): void;
    disconnect(slot: (...args: any[]) => any): void;
    (): void;
  }

  export interface Slot {
    (...args: any[]): any;
    [p: string]: any;
  }

  export interface Method {
    (): void;
  }

  export type QtObject<T> = { objectName: string } & {
    [P in keyof T]: T[P];
  };

  export namespace QtQml {
    export type Timer = QtObject<{
      interval: number;
      repeat: boolean;
      running: boolean;
      triggeredOnStart: boolean;
      triggered: Signal;
      restart: Method;
      start: Method;
      stop: Method;
    }>;
  }
}
