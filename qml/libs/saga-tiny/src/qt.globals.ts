declare var Qt: QtGlobalType;

interface QtGlobalType {
  callLater<Fn extends (...args: any[]) => any>(
    callback: Fn,
    ...args: Parameters<Fn>
  ): void;
  setTimeout<Fn extends (...args: any[]) => any>(
    handler: Fn,
    ms: number
  ): number;
  clearTimeout(handle: number): void;
  [key: string]: any;
}

if (!Qt) {
  Qt = {
    callLater: setImmediate,
    setTimeout: setTimeout as any,
    clearTimeout: clearTimeout as any,
  };
}
