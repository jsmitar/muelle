declare module '@qml/org.muelle.types-1.0' {
  import Qt from 'qt';

  export namespace Types {
    export type Edge = 2 | 4 | 8 | 16;
    export const Top = 2;
    export const Left = 4;
    export const Bottom: 8;
    export const Right: 16;

    export type Orientation = 32 | 64;
    export const Horizontal: 32;
    export const Vertical: 64;

    export type Alignment = 128 | 256 | 512;
    export const Start: 128;
    export const Center: 256;
    export const End: 512;

    export type Behavior = 0 | 1 | 2 | 3 | 4;
    export const None: 0;
    export const AlwaysVisible: 1;
    export const AutoHide: 2;
    export const DodgeAll: 3;
    export const DodgeActive: 4;
  }

  export namespace Dock {
    export interface $view extends Qt.QtObject {
      readonly containsMouse: boolean;
      mask: Qt.rect;
      readonly geometry: Qt.rect;
      panelGeometry: Qt.rect;
      size: Qt.size;
      position: Qt.point;
      mousePosition: Qt.point;
      setOpacity(level: number): void;
    }

    export interface $layout extends Qt.QtObject {
      edege: Types.Edge;
      readonly orientation: Types.Orientation;
      alignment: Types.Alignment;
      readonly layout: number;
      readonly isHorizontal: boolean;
      readonly isVertical: boolean;
    }

    export interface $positioner extends Qt.QtObject {
      centerOffset: number;
      update(duration: number): void;
    }
  }
}
