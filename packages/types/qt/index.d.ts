export = Qt;
export as namespace Qt;

import { type } from './symbols';
import QtQml from '@qml/QtQml-2.14';
import { Dock } from '@qml/org.muelle.types-1.0';

declare namespace Qt {
  //START: Qt Namespace
  type KeyboardModifier = enumeration<'KeyboardModifier'>;
  const NoModifier: KeyboardModifier;
  const ShiftModifier: KeyboardModifier;
  const ControlModifier: KeyboardModifier;
  const AltModifier: KeyboardModifier;
  const MetaModifier: KeyboardModifier;
  const KeypadModifier: KeyboardModifier;
  const GroupSwitchModifier: KeyboardModifier;
  //END: Qt Namespace

  //START: No standard, extensions and globals
  class Timer {}
  const setTimeout: (handler: () => any, ms: number) => Timer;
  const clearTimeout: (timer: Timer) => void;
  const $positioner: Dock.$positioner;
  const $view: Dock.$view;
  const $layout: Dock.$layout;
  //END: No standard

  //START: Qt QML Type
  function isQtObject(object: any): boolean;
  function callLater<Fn extends (...args: any[]) => any>(
    callback: Fn,
    ...args: Parameters<Fn>
  ): void;
  function createComponent<T extends QtObject = QtObject>(
    url: string,
    mode?: QtQml.Component.Status
  ): QtQml.Component<T>;
  function rect(x: number, y: number, width: number, height: number): rect;
  function size(width: number, height: number): size;
  function point(x: number, y: number): point;
  function rgba(red: number, green: number, blue: number, alpha: number): color;
  //END: Qt QML Type

  //START: Basic Qt Data Types
  type enumeration<T extends string> = number & {
    [type]: T;
  };
  type url = string;
  type rect = size &
    point & {
      left: number;
      right: number;
      top: number;
      bottom: number;
    };

  type size = {
    width: number;
    height: number;
  };

  type point = {
    x: number;
    y: number;
  };

  type color = {
    r: number;
    g: number;
    b: number;
    a: number;
    hsvHue: number;
    hsvSaturation: number;
    hsvValue: number;
    hslHue: number;
    hslSaturation: number;
    hslLightness: number;
  };
  //END: Basic Qt Data Types

  interface Signal<
    Fn extends (...args: any[]) => any = (...args: any[]) => any
  > {
    connect(slot: Fn): void;
    disconnect(slot: Fn): void;
    (): void;
  }

  interface Slot {
    (...args: any[]): any;
    [p: string]: any;
  }

  interface Method {
    (): void;
  }

  type QtObject<Properties = {}> = {
    objectName: string;
    destroy(delay?: number): void;
  } & {
    [P in keyof Properties]: Properties[P];
  };
}
