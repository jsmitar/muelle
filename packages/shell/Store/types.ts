import { Types } from '@qml/org.muelle.types-1.0';
import { ReadonlyDeep } from 'Object/Readonly';
import { TasksModel } from 'plasma/org.kde.plasma.private.taskmanager-0.1';
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
  tasksModel: TasksModel;
}

export interface Settings extends Qt.QtObject<{}> {
  visible: boolean;
}

export interface Panel extends Qt.QtObject<{}> {
  lockVisible: boolean;
  visible: boolean;
  readonly hidden: boolean;

  slide: string;

  updatingOffset: boolean;
  updatingOrientation: boolean;
  updatingScreen: any;
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
  separatorCount: number;
}

export interface Icon extends Qt.QtObject {
  size: number;
  readonly iconSize: number;
  spacing: number;
  padding: number;
}

export interface Geometry extends Qt.QtObject {
  readonly view: Qt.QtObject<
    Readonly<{
      width: number;
      height: number;
      size: number;
    }>
  >;
  readonly background: QtObject<
    ReadonlyDeep<{
      shadow: number;
      paddingX: number;
      paddingTop: number;
      inset: number;
      extents: number;
      isAtStart: boolean;
      isAtEnd: boolean;
      x: number;
      y: number;
      width: number;
      height: number;
      boxHeight: number;
      point: Qt.point;
      size: Qt.size;
      rect: Qt.rect;
      next: QtObject<{
        x: number;
        y: number;
        width: number;
        height: number;
        point: Qt.point;
        size: Qt.size;
        rect: Qt.rect;
      }>;
    }>
  >;
  panel: QtObject<
    ReadonlyDeep<{
      offset: number;
      xCenter: number;
      x: number;
      y: number;
      width: number;
      height: number;
      point: Qt.point;
      size: Qt.size;
      rect: Qt.rect;
      next: QtObject<{
        xCenter: number;
        x: number;
        y: number;
        width: number;
        height: number;
        point: Qt.point;
        size: Qt.size;
        rect: Qt.rect;
      }>;
    }>
  >;

  mask: QtObject<
    {
      growing: boolean;
      enabled: boolean;
      fill: boolean;
    } & ReadonlyDeep<{
      region: Qt.region;
      visible: QtObject<{
        region: Qt.region;
        regionFill: Qt.region;
      }>;
      hidden: QtObject<{
        region: Qt.region;
      }>;
    }>
  >;
}

export interface Animation extends Qt.QtObject {
  duration: number;
  velocity: number;
  addEnabled: boolean;
}

export interface Configuration extends Qt.QtObject {
  save(): void;
  [k: string]: any;
}

export interface Context extends QtObject {
  view: Dock.view;
  layout: Dock.layout;
  positioner: Dock.positioner;
  container: Dock.container;
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
    screen: Qt.QScreen;
    setOpacity(level: number): void;
    release: Qt.Signal;
    loaded: Qt.Signal;
  }

  export interface layout extends Qt.QtObject {
    edge: Types.Edge;
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

  export interface container extends Qt.QtObject {
    screens: { [screen: string]: Qt.QScreen };
  }
}
