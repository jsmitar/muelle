declare module '@qml/org.muelle.types-1.0' {
  export namespace Types {
    export type Edge = 2 | 4 | 8 | 16;
    export const Top = 2;
    export const Left = 4;
    export const Bottom: 8;
    export const Right: 16;

    export type Orientation = 10 | 20;
    export const Horizontal: 10;
    export const Vertical: 20;

    export type Alignment = 32 | 64 | 128;
    export const Start: 32;
    export const Center: 64;
    export const End: 128;

    export type Behavior = 0 | 1 | 2 | 3 | 4;
    export const None: 0;
    export const AlwaysVisible: 1;
    export const AutoHide: 2;
    export const DodgeAll: 3;
    export const DodgeActive: 4;
  }
}
