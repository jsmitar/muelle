import Qt from 'qt';

declare module 'plasma/org.kde.plasma.private.taskmanager-0.1' {
  export interface TaskItemModel extends Qt.QtObject {
    index: number;
    row: number;
    column: number;
    hasModelChildren: boolean;
    GenericName: string;
    LauncherUrl: {};
    LauncherUrlWithoutIcon: {};
    WinIdList: number[];
    MimeType: string;
    MimeData: {};
    IsWindow: boolean;
    IsStartup?: boolean;
    IsLauncher?: boolean;
    HasLauncher: boolean;
    display: string;
    IsGroupParent: boolean;
    decoration: object;
    ChildCount: undefined;
    edit: undefined;
    IsGroupable: boolean;
    toolTip: undefined;
    IsActive: boolean;
    statusTip: undefined;
    whatsThis: undefined;
    IsClosable: boolean;
    IsMovable: boolean;
    IsResizable: boolean;
    IsMaximizable: boolean;
    IsMaximized: boolean;
    IsMinimizable: boolean;
    IsMinimized: boolean;
    IsKeepAbove: boolean;
    IsKeepBelow: boolean;
    IsFullScreenable: boolean;
    IsFullScreen: boolean;
    IsShadeable: boolean;
    IsShaded: boolean;
    IsVirtualDesktopsChangeable: boolean;
    VirtualDesktops: number[];
    IsOnAllVirtualDesktops: boolean;
    Geometry: Qt.rect;
    ScreenGeometry: Qt.rect;
    Activities: Record<string, string>;
    IsDemandingAttention: boolean;
    SkipTaskbar: boolean;
    SkipPager: boolean;
    AppPid: number;
    AppId: string;
    AppName: string;
    objectNameChanged: Qt.Signal;
    modelIndexChanged: Qt.Signal;
    rowChanged: Qt.Signal;
    columnChanged: Qt.Signal;
  }
}
