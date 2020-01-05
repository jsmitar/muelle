import QtQuick 2.11
import QtQml.Models 2.3
import org.muelle.types 1.0
import org.kde.taskmanager 0.1 as TaskManager
import QtQml.Models 2.12
import '../../shared/functional.ts' as F
import '../libs/taskHelper.ts' as TaskHelper
import '../libs/constants.ts' as Consts
import "../Components"
import "../Extras"

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

  property var m: model
  property Item panel
  property int index: model.index

  property int size: store.state.icon.size
  property int spacing: store.state.icon.spacing
  property int padding: store.state.icon.padding

  property bool hold: false
  property Item iconTarget
  property DelegateModel delegateModel: panel.model

  hoverEnabled: true
  acceptedButtons: Qt.LeftButton | Qt.RightButton
  width: size
  height: size

  //! BEGIN: Drag&Drop
  z: containsMouse || drag.active ? 1 : 0
  drag {
    axis: Drag.XAndYAxis
    smoothed: false
    threshold: 0
    filterChildren: true
    target: iconTarget
  }

  drag.onActiveChanged: {
    if (drag.active) {
      opacity = 0.5
      drag.target.grabToImage(img => {
        hold = true
        panel.Drag.imageSource = img.url
        panel.dragging = task
      })
    } else {
      panel.dragging = null
      hold = false
      opacity = 1
    }
  }


  Component.onDestruction: {
    panel.dragging = null
  }
  //! END: Drag&Drop

  //! BEGIN: MouseHandlers
  onEntered: {
    panel.hovered = task
  }

  onExited: {
    panel.hovered = null
  }

  onReleased: {
    if (panel.dragging) return

   const action = {
      [Qt.LeftButton]() {
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
      width: size
      height: parent.height
    }
    vertical: PropertyChanges {
      target: task
      width: parent.width
      height: size
    }
  }

  StateLayoutEdge {
    reset: [
      AnchorChanges {
        target: iconTarget
        anchors {
          top: undefined
          right: undefined
          bottom: undefined
          left: undefined
          horizontalCenter: undefined
          verticalCenter: undefined
        }
      }
    ]
    top: AnchorChanges {
      target: iconTarget
      anchors.top: task.top
      anchors.horizontalCenter: task.horizontalCenter
    }
    right: AnchorChanges {
      target: iconTarget
      anchors.right: task.right
      anchors.verticalCenter: task.verticalCenter
    }
    bottom: AnchorChanges {
      target: iconTarget
      anchors.bottom: task.bottom
      anchors.horizontalCenter: task.horizontalCenter
    }
    left: AnchorChanges {
      target: iconTarget
      anchors.left: task.left
      anchors.verticalCenter: task.verticalCenter
    }
  }

}
