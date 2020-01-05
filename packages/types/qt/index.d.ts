export = Qt;
export as namespace Qt;

import { type } from './symbols';
import QtQml from '@qml/QtQml-2.14';

declare namespace Qt {
  //START: Qt Namespace
  export type KeyboardModifier = enumeration<'KeyboardModifier'>;
  export const NoModifier: KeyboardModifier;
  export const ShiftModifier: KeyboardModifier;
  export const ControlModifier: KeyboardModifier;
  export const AltModifier: KeyboardModifier;
  export const MetaModifier: KeyboardModifier;
  export const KeypadModifier: KeyboardModifier;
  export const GroupSwitchModifier: KeyboardModifier;
  //END: Qt Namespace

  //START: No standard
  export const Promise: PromiseConstructor;
  //END: No standard

  //START: Qt QML Type
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
  export type enumeration<T extends string> = number & {
    [type]: T;
  };
  export type url = string;
  export type rect = size &
    point & {
      left: number;
      right: number;
      top: number;
      bottom: number;
    };

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
    a: number;
    hsvHue: number;
    hsvSaturation: number;
    hsvValue: number;
    hslHue: number;
    hslSaturation: number;
    hslLightness: number;
  };
  //END: Basic Qt Data Types

  export interface Signal<
    Fn extends (...args: any[]) => any = (...args: any[]) => any
  > {
    connect(slot: Fn): void;
    disconnect(slot: Fn): void;
    (): void;
  }

  export interface Slot {
    (...args: any[]): any;
    [p: string]: any;
  }

  export interface Method {
    (): void;
  }

  export type QtObject<T = any> = {
    objectName: string;
    destroy(delay?: number): void;
  } & {
    [P in keyof T]: T[P];
  };
}
