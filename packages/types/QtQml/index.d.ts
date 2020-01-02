declare module '@qml/QtQml-2.x' {
  import Qt from 'qt';
  export interface Timer extends Qt.QtObject {
    interval: number;
    repeat: boolean;
    running: boolean;
    triggeredOnStart: boolean;
    triggered: Qt.Signal;
    restart: Qt.Method;
    start: Qt.Method;
    stop: Qt.Method;
  }
  export interface Component<T extends Qt.QtObject> {
    progress: number;
    status: Component.Status;
    url: Qt.url;
    completed: Qt.Signal;
    destruction: Qt.Signal;
    createObject(
      parent: Qt.QtObject<any> | null,
      properties: Partial<T>
    ): T | null;
    errorString(): string;
    incubateObject(
      parent: Qt.QtObject | null,
      properties: Partial<T>
    ): Incubator<T> | null;
  }
  export type Incubator<T> = {
    status: Component.Status;
    object: T;
    onStatusChanged: Qt.Signal<(status: Component.Status) => any>;
    forceCompletion: Qt.Method;
  };
  export namespace Component {
    export type Mode = Qt.enumeration<'Mode'>;
    export const PreferSynchronous: Mode;
    export const Asynchronous: Mode;

    export type Status = Qt.enumeration<'Status'>;
    export const Null: Status;
    export const Ready: Status;
    export const Loading: Status;
    export const Error: Status;
  }
}
