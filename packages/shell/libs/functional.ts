'.pragma library';

import Qt from 'qt';
import { Timer } from '@qml/QtQml-2.x';

export function randColor(alpha: number) {
  alpha = alpha || 1;
  return Qt.rgba(
    0.5 + Math.random() / 2,
    0.5 + Math.random() / 2,
    0.5 + Math.random() / 2,
    alpha
  );
}

export function alpha(color: Qt.color, alpha: number) {
  return Qt.rgba(color.r, color.g, color.b, alpha);
}

export function clamp(min: number, value: number, max: number) {
  return value > max ? max : value < min ? min : value;
}

export function identity(item: number) {
  return item;
}

export function noop() {}

export function each<T>(
  array: Iterable<T>,
  fn: (value: T, index: number, array: T[]) => void
) {
  return Array.prototype.forEach.call(array, fn);
}

export function map<T, U>(
  iterable: T[],
  fn: (value: T, index: number, array: T[]) => U
) {
  return Array.prototype.map.call<T[], any, U[]>(iterable, fn);
}

export function reduce<T, U>(
  array: T[],
  fn: (
    previousValue: U,
    currentValue: T,
    currentIndex: number,
    array: T[]
  ) => U,
  initial: U
) {
  return Array.prototype.reduce.call<T[], any, U>(array, fn, initial);
}

export function filter<T>(
  array: T[],
  predicate: (value: T, index: number, array: T[]) => boolean
) {
  return Array.prototype.filter.call<T[], any, T[]>(array, predicate);
}

export function intersect<T, U>(arrayL: T[], arrayR: U[]): T[] {
  return Array.prototype.filter.call(arrayL, item => {
    return Array.prototype.includes.call(arrayR, item);
  });
}

export function zip<T, U>(arrayL: T[], arrayR: U[]) {
  const shorter = arrayL.length < arrayR.length ? arrayL : arrayR;

  return Array.prototype.map.call<any[], any, [T, U][]>(
    shorter,
    (_: any, i: number) => {
      return [arrayL[i], arrayR[i]];
    }
  );
}

export function flat<T>(array: T[][]) {
  return Array.prototype.reduce.call<T[][], any, T[]>(
    array,
    (arr: T[], item: T[]) => arr.concat(item),
    []
  );
}

export function filterUntil<T>(
  array: T[],
  predicate: (item: T, index: number, array: T[]) => boolean
) {
  const filtered = [];

  predicate = predicate || no;
  for (let i = 0; i < array.length; i++) {
    if (predicate(array[i], i, array)) {
      filtered.push(array[i]);
    } else {
      break;
    }
  }

  return filtered;
}

export function range(start = 0, stop: number = 0, step: number = 1) {
  if (stop === undefined) {
    stop = start;
    start = 0;
  }

  return {
    *[Symbol.iterator]() {
      for (let i = start; i !== stop; i += step) yield i;
    },
  };
}

export function timeout(timer: Timer, slot: Qt.Slot, interval?: number) {
  function disconnect() {
    if (timer && timer.triggered) {
      timer.triggered.disconnect(slot);
      timer.stop();
    }
  }

  timer.stop();
  timer.interval = interval !== undefined ? interval : timer.interval;
  timer.triggered.connect(slot);
  timer.triggered.connect(disconnect);
  timer.start();
  return disconnect;
}

export function asyncTimeout(interval: number) {
  return new Qt.Promise(r => Qt.setTimeout(r, interval));
}

export function yes() {
  return true;
}

export function no() {
  return false;
}

export function isString(value: any): value is string {
  return typeof value === 'string' || value instanceof String;
}

function isNumber(value: any): value is number {
  return typeof value === 'number' || value instanceof Number;
}

function isCallable(value: any): value is (...args: any) => any {
  return typeof value === 'function';
}

function isObject(value: any): value is Record<any, any> {
  return value && typeof value === 'object';
}

function isPoint(value: any): value is Qt.point {
  return `${value}`.startsWith('QPoint');
}

function isSize(value: any): value is Qt.size {
  return `${value}`.startsWith('QSize');
}

function isRect(value: any): value is Qt.rect {
  return `${value}`.startsWith('QRect');
}

function isArray(value: any): value is any[] {
  return Array.isArray(value);
}

function quote(str: any) {
  return `"${str}"`;
}

function space(length: number) {
  return repeat(' ', length);
}

function formatHelper(
  str: string,
  [left, right]: [string, string],
  indent: number
) {
  return `${left}${breakLine(indent)}${space(indent)}${str}${breakLine(
    indent
  )}${space(indent - 2)}${right}`;
}

function breakLine(indent: number) {
  return indent >= 0 ? '\n' : ' ';
}

function atostr(array: any[], deep = 5, indent = 2): string {
  if (deep === 0) {
    return '[Array]';
  }

  if (!array.length) {
    return '[]';
  }

  const str = array
    .map(v => `${tostr(v, deep - 1, indent + 2)}`)
    .join(`,${breakLine(indent)}${space(indent)}`);

  return formatHelper(str, ['[', ']'], indent);
}

function otostr(object: Record<any, any>, deep = 5, indent = 2): string {
  if (deep === 0) {
    const ostr = String(object) || 'Object';
    return `${ostr}${object.objectName ? '-' + object.objectName : ''}`;
  }

  if (!Object.keys(object).length) {
    return '{}';
  }

  const str = Object.entries(object)
    .filter(([k]) => !k.startsWith('_'))
    .map(([k, v]) => {
      let nextDeep = deep - 1;
      if (k === 'parent' || object[k] === object.parent || v === object) {
        nextDeep = 0;
      }
      return `${k}: ${tostr(v, nextDeep, indent + 2)}`;
    })
    .join(`,${breakLine(indent)}${space(indent)}`);

  return formatHelper(str, ['{', '}'], indent);
}

function ctostr(callable: (...args: any[]) => any): string {
  if (callable.name) {
    return `[Function: ${callable.name}]`;
  }
  return '[Function]';
}

function repeat(value: string, length = 1) {
  return Array.from({ length: length + 1 }).join(value);
}

function tostr(value: any, deep = 5, indent = 2) {
  if (isString(value)) {
    return quote(value);
  }
  if (isNumber(value)) {
    return Number(value.toFixed(3));
  }
  if (isArray(value)) {
    return atostr(value, deep, indent);
  }
  if (isCallable(value)) {
    return ctostr(value);
  }
  if (isPoint(value)) {
    return otostr(
      {
        x: value.x,
        y: value.y,
      },
      deep,
      indent
    );
  }
  if (isSize(value)) {
    return otostr(
      {
        width: value.width,
        height: value.height,
      },
      deep,
      indent
    );
  }
  if (isRect(value)) {
    return otostr(
      {
        x: value.x,
        y: value.y,
        width: value.width,
        height: value.height,
      },
      deep,
      indent
    );
  }
  if (isObject(value)) {
    return otostr(value, deep, indent);
  }
  if (!value) {
    return value;
  }

  return String(value);
}

export function singleConnect(signal: Qt.Signal, slot: Qt.Slot) {
  try {
    signal.connect(function _slot(...args: any[]) {
      signal.disconnect(_slot);
      slot.apply(null, args);
    });
  } catch (e) {
    console.warn(e);
  }
}

export function on(signal: Qt.Signal, slot: Qt.Slot) {
  try {
    signal.connect(slot);
    return () => signal.disconnect(slot);
  } catch (e) {
    console.warn(e);
  }

  return noop;
}

/**
 * @returns {[]}
 * args args (n-1) are signals
 * slot: last argument is considered a slot
 */
export function once(...args: any[]) {
  if (args.length < 2) throw new Error('insufficient args');

  try {
    const signals = args.slice(0, -1);
    const slot = args[args.length - 1];

    if (slot) {
      const clear = () => signals.forEach(signal => signal.disconnect(slot));

      signals.forEach(signal => {
        signal.connect(slot);
        signal.connect(clear);
      });
      return slot;
    }
  } catch (e) {
    console.warn(e);
  }
}

/**
 * args: args (n-1) are signals
 * slot: last argument is considered a slot
 */
export function disconnect(...args: any[]) {
  if (args.length < 2) throw new Error('insufficient args');

  try {
    const signals = args.slice(0, -1);
    const slot = args[args.length - 1];

    if (slot) signals.forEach(signal => signal.disconnect(slot));
  } catch (e) {
    console.warn(e);
  }
}

export class CanceledError extends Error {
  constructor(message = 'CanceledError') {
    super(message);
  }
}

export function makeCancelable<Fn extends (...args: any[]) => any>(
  fnPromise: Fn
) {
  let cancel = noop;

  const cancelPromise = new Qt.Promise((_, reject) => {
    cancel = () => reject(new CanceledError());
  });

  return {
    cancel,
    job(...args: Parameters<Fn>) {
      return Qt.Promise.race([
        cancelPromise,
        fnPromise.call({ cancel }, ...args),
      ]);
    },
  };
}

export function debounce<Fn extends (...args: any[]) => any>(
  fn: Fn,
  time: number
) {
  let timeout = 0;
  return (...args: Parameters<Fn>) => {
    const lastArgs = args;
    Qt.clearTimeout(timeout);
    timeout = Qt.setTimeout(() => fn(...lastArgs), time);
  };
}
