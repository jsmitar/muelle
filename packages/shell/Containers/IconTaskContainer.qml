import QtQuick 2.14
import QtQml.Models 2.12
import org.muelle.types 1.0
import org.kde.taskmanager 0.1 as TaskManager

import 'qrc:/shared/components'
import 'qrc:/shared/functional.ts' as F
import 'qrc:/shell/helpers/taskHelper.ts' as TaskHelper
import 'qrc:/shell/helpers/constants.ts' as Consts
import 'qrc:/shell/Components'

/*!
  \code model:
{
  "objectName": "",
  "index": 2,
  "row": 2,
  "column": 0,
  "model": [object QQmlDMAbstractItemModelData(0x3b5d520)],
  "hasModelChildren": false,
  "GenericName": "The IDE of choice for Qt development.",
  "LauncherUrl": {},
  "LauncherUrlWithoutIcon": {},
  "WinIdList": [
    60817420
  ],
  "MimeType": "windowsystem/winid",
  "MimeData": {},
  "IsWindow": true,
  "IsStartup": undefined,
  "IsLauncher": undefined,
  "HasLauncher": false,
  "display": "PaintItem.qml (qml/Extras @ Muelle) - Qt Creator",
  "IsGroupParent": undefined,
  "decoration": {},
  "ChildCount": undefined,
  "edit": undefined,
  "IsGroupable": true,
  "toolTip": undefined,
  "IsActive": true,
  "statusTip": undefined,
  "whatsThis": undefined,
  "IsClosable": true,
  "IsMovable": true,
  "IsResizable": true,
  "IsMaximizable": true,
  "IsMaximized": true,
  "IsMinimizable": true,
  "IsMinimized": false,
  "IsKeepAbove": false,
  "IsKeepBelow": false,
  "IsFullScreenable": false,
  "IsFullScreen": false,
  "IsShadeable": true,
  "IsShaded": false,
  "IsVirtualDesktopsChangeable": true,
  "VirtualDesktops": [1],
  "IsOnAllVirtualDesktops": false,
  "Geometry": {
    "x": 0,
    "y": 30,
    "width": 1366,
    "height": 738,
    "left": 0,
    "right": 1365,
    "top": 30,
    "bottom": 767
  },
  "ScreenGeometry": {
    "x": 0,
    "y": 0,
    "width": 1366,
    "height": 768,
    "left": 0,
    "right": 1365,
    "top": 0,
    "bottom": 767
  },
  "Activities": {
    "0": "788cecfc-9913-4be4-8826-1bed5ac833f0"
  },
  "IsDemandingAttention": false,
  "SkipTaskbar": false,
  "SkipPager": false,
  "AppPid": 7480,
  "AppId": "DigiaQt-qtcreator-community.desktop",
  "AppName": "Qt Creator (Community)",
  "objectNameChanged": [Function],
  "modelIndexChanged": [Function],
  "rowChanged": [Function],
  "columnChanged": [Function]
}
/endcode
*/

MouseArea {
  id: task
  objectName: 'IconTaskContainer'

  property Item panel

  readonly property int spacing: store.state.icon.spacing
  readonly property int padding: store.state.icon.padding

  property bool hold: false
  
  property bool inPopup: false
  readonly property Item itemDragTarget: taskItem.item.target

  property var m: model
  readonly property int index: m.index
  readonly property bool isStartup: !!m.IsStartup
  readonly property bool isSeparator: m.GenericName === '__separator__'
  readonly property bool smartLauncherEnabled: !inPopup && isStartup && !isSeparator

  property DelegateModel delegateModel: panel.model

  hoverEnabled: true
  acceptedButtons: Qt.LeftButton | Qt.RightButton

  opacity: panel.dragging === task ? 0.5 : 1

  Component.onCompleted: { 
    TaskHelper.publishGeometry(store.tasksModel, $view.position, task)
    const model = store.tasksModel.makeModelIndex(m.index)
  }

  Component {
    id: iconTask
    IconTask {
      m: model
      hover: task.containsMouse
    }
  }

  Component {
    id: separatorTask
    SeparatorTask {
      m: model
    }
  }

  Loader {
    id: taskItem

    sourceComponent: isSeparator ? separatorTask : iconTask
  }

  //! BEGIN: Drag&Drop
  z: containsMouse || drag.active ? 1 : 0
  drag {
    axis: Drag.XAndYAxis
    smoothed: false
    threshold: 5
    filterChildren: true
    target: itemDragTarget
  }

  drag.onActiveChanged: {
    if (drag.active) {
      drag.target.grabToImage(img => {
        hold = true
        panel.dragging = task
        panel.removedLauncher = ''
        panel.Drag.imageSource = img.url
        panel.Drag.active = true
      })
    } else {
      Qt.callLater(() => {
        hold = false
      })
    }
  }
  //! END: Drag&Drop

  //! BEGIN: MouseHandlers
  onEntered: {
    panel.hovered = task
  }

  onExited: {
    if (panel.hovered === task)
      panel.hovered = null
  }

  onReleased: {
    if (hold) return

   const action = {
      [Qt.LeftButton]() {
        TaskHelper.publishGeometry(store.tasksModel, $view.position, task)
        TaskHelper.activate(store.tasksModel, task, mouse)
      },
      [Qt.RightButton]() {
        store.contextMenu.show()
      }
    }[mouse.button]

    action()
  }

  onWheel: {
    console.log(F.tostr(wheel.angleDelta))
  }
  //! END: MouseHandlers

  //! BEGIN: Animations
  Behavior on opacity {
    OpacityAnimator {
      duration: 150
    }
  }
  //! END: Animations

  StateLayoutOrientation {
    horizontal: PropertyChanges {
      target: task
      width: taskItem.width
      height: parent.height
    }
    vertical: PropertyChanges {
      target: task
      width: parent.width
      height: taskItem.height
    }
  }

  StateLayoutEdge {
    reset: AnchorChanges {
      target: taskItem
      anchors {
        top: undefined
        right: undefined
        bottom: undefined
        left: undefined
        horizontalCenter: undefined
        verticalCenter: undefined
      }
    }
    top: AnchorChanges {
      target: taskItem
      anchors.top: task.top
      anchors.horizontalCenter: task.horizontalCenter
    }
    right: AnchorChanges {
      target: taskItem
      anchors.right: task.right
      anchors.verticalCenter: task.verticalCenter
    }
    bottom: AnchorChanges {
      target: taskItem
      anchors.bottom: task.bottom
      anchors.horizontalCenter: task.horizontalCenter
    }
    left: AnchorChanges {
      target: taskItem
      anchors.left: task.left
      anchors.verticalCenter: task.verticalCenter
    }
  }
}
