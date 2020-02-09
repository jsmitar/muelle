import { Types } from '@qml/org.muelle.types-1.0';
import Qt, { QtObject } from 'qt';
import { O } from 'ts-toolbelt';

export interface ShellStateReadonly
  extends O.Readonly<ShellState, any, 'deep'> {}

export interface ShellState extends Qt.QtObject {
  settings: Settings;
  panel: Panel;
  icon: Icon;
  geometry: Geometry;
  animation: Animation;
  context: Context;
}

export interface Settings extends Qt.QtObject<{}> {
  visible: boolean;
}

export interface Panel extends Qt.QtObject<{}> {
  lockVisible: boolean;
  visible: boolean;
  readonly hidden: boolean;

  slide: string;

  updatingPosition: boolean;
  nextEdge: Types.Edge;
  nextAlignment: Types.Alignment;
  alignment: Types.Alignment;
  edge: Types.Edge;
  behavior: Types.Behavior;

  readonly orientation: Types.Orientation;
  readonly isHorizontal: boolean;
  readonly isVertical: boolean;

  nextTaskCount1: number;
  taskCount1: number;
}

export interface Icon extends Qt.QtObject {
  size: number;
  readonly iconSize: number;
  spacing: number;
  padding: number;
}

export interface Geometry extends Qt.QtObject {
  readonly viewSize: Qt.size;

  readonly panelSize: Qt.size;
  readonly panelRect: Qt.rect;

  readonly panelNextRect: Qt.rect;
  readonly panelNextPoint: Qt.point;

  readonly panelAbsoluteRect: Qt.rect;
  readonly panelNextAbsoluteRect: Qt.rect;

  readonly maskRect: Qt.rect;
  maskGrowing: boolean;
  maskEnabled: boolean;
}

export interface Animation extends Qt.QtObject {
  duration: number;
  velocity: number;
}

export interface Configuration extends Qt.QtObject {
  save(): void;
  [k: string]: any;
}

export interface Context extends QtObject {
  view: Dock.view;
  layout: Dock.layout;
  positioner: Dock.positioner;
  configuration: Configuration;
}

export namespace Dock {
  export interface view extends Qt.QtObject {
    readonly containsMouse: boolean;
    mask: Qt.rect;
    readonly geometry: Qt.rect;
    panelGeometry: Qt.rect;
    size: Qt.size;
    position: Qt.point;
    mousePosition: Qt.point;
    setOpacity(level: number): void;
  }

  export interface layout extends Qt.QtObject {
    edege: Types.Edge;
    readonly orientation: Types.Orientation;
    alignment: Types.Alignment;
    readonly layout: number;
    readonly isHorizontal: boolean;
    readonly isVertical: boolean;
  }

  export interface positioner extends Qt.QtObject {
    centerOffset: number;
    update(duration: number): void;
  }
}
