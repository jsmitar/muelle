import Qt from 'qt';
import { Types } from '@qml/org.muelle.types-1.0';

export const CHANGE_EDGE = 'CHANGE_EDGE';
export const CHANGE_ALIGNMENT = 'CHANGE_ALIGNMENT';
export const HIDE_PANEL = 'HIDE_PANEL';
export const SHOW_PANEL = 'SHOW_PANEL';
export const LOCK_VISIBLE = 'LOCK_VISIBLE';

export const SLIDE_STATUS = 'SLIDE_STATUS';

export const NEXT_EDGE = 'NEXT_EDGE';
export const NEXT_ALIGNMENT = 'NEXT_ALIGNMENT';
export const NEXT_BEHAVIOR = 'NEXT_BEHAVIOR';
export const UPDATING_POSITION = 'UPDATING_POSITION';
export const UPDATE_POSITION = 'UPDATE_POSITION';

export const SHOW_SETTINGS = 'SHOW_SETTINGS';
export const HIDE_SETTINGS = 'HIDE_SETTINGS';

export const START_ANIMATION = 'START_ANIMATION';

export const MASK_GROW = 'MASK_GROW';

export const NEXT_TASK_COUNT1 = 'NEXT_TASK_COUNT1';
export const UPDATE_TASK_COUNT_1 = 'UPDATE_TASK_COUNT_1';

export interface ShellState extends Qt.QtObject {
  settings: Settings;
  panel: Panel;
  icon: Icon;
  geometry: Geometry;
  animation: Animation;
}

export interface Settings extends Qt.QtObject<{}> {
  visible: false;
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
  readonly _panel_width: number;
  readonly _panel_height: number;
  readonly _panel_x: number;
  readonly _panel_y: number;
  readonly _panel_size: Qt.size;
  readonly _panel_point: Qt.point;

  readonly _panel_next_width: number;
  readonly _panel_next_height: number;
  readonly _panel_next_x: number;
  readonly _panel_next_y: number;
  readonly _panel_next_size: Qt.size;
  readonly _panel_next_point: Qt.point;

  readonly viewSize: Qt.size;
  readonly viewRect: Qt.rect;

  readonly panelSize: Qt.size;
  readonly panelPoint: Qt.point;
  readonly panelRect: Qt.rect;

  readonly panelNextSize: Qt.size;
  readonly panelNextPoint: Qt.point;
  readonly panelNextRect: Qt.rect;

  readonly panelAbsoluteRect: Qt.rect;
  readonly panelNextAbsoluteRect: Qt.rect;

  maskGrow: boolean;
  readonly _hidden: boolean;
  readonly maskSizeVisible: Qt.size;
  readonly maskPointVisible: Qt.point;
  readonly maskSizeHidden: Qt.size;
  readonly maskPointHidden: Qt.point;

  readonly maskRect: Qt.rect;
}

export interface Animation extends Qt.QtObject {
  duration: number;
  velocity: number;
}
