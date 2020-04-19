import QtQuick 2.12
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import org.muelle.types 1.0

import '../Store/actions.ts' as Action
import '../../shared/components'
import '../Components'
import '../../shared/functional.ts' as F
import '../../shared/sequence.ts' as Seq

Loader {
  id: panelContainer
  objectName: 'PanelContainer'

  asynchronous: false
  sourceComponent: $layout.isHorizontal ? horizontal : vertical

  default property DelegateModel model
  property Item hovered
  property Item dragging
  property Item positioner

  property bool animMoveEnabled: true

  Drag.active: false
  Drag.hotSpot.x: dragging ? dragging.width / 2 : 0
  Drag.hotSpot.y: dragging ? dragging.height / 2 : 0
  Drag.dragType: Drag.Automatic

  Repeater { parent: positioner; model: panelContainer.model }

  DropArea {
    id: dropArea

    readonly property rect rect: store.state.geometry.panelNextRect

    anchors {
      fill: parent
      topMargin: rect.y
      rightMargin: parent.width - rect.width - rect.x
      bottomMargin: parent.height - rect.height - rect.y
      leftMargin: rect.x
    }

    function syncLaunchers() {
      animMoveEnabled = false 
      store.tasksModel.syncLaunchers()
      Qt.setTimeout(() => {
        animMoveEnabled = true  
      }, 500)
    }

    function moveTask(dragging, hovered) {
      if (dragging && hovered) {
        const { move } = store.tasksModel
        move(dragging.index, hovered.index)
      }
    }

    property var positionHandler: F.throttle(() => {
      const item = positioner.childAt(drag.x, drag.y)

      const appId = item ? item.m.AppId : ''
      const prevAppId = hovered ? hovered.m.AppId : ''

      if (prevAppId !== appId) {
        hovered = item
        moveTask(dragging, item)
      }
    }, 250)

    drag.onPositionChanged: positionHandler()

    onEntered: {
      // TODO: Verify if is valid
      // const apps = Seq
      //   .from(drag.urls)
      //   .filter(url => store.backend.isApplication(url))
      
      // if (apps.length) {
      //   drag.accept()
      // } else {
      //   drag.ignore()
      // }
    }

    onExited: {
      hovered = null
    }

    onDropped: {
      const allLaunchers = F.every(
        drop.urls, 
        url => store.backend.isApplication(url)
      )

      if (allLaunchers) {
        F.each(drop.urls, url => {
          store.tasksModel.requestAddLauncher(url)
        })
      } else if (hovered) {
        const modelIndex = store.tasksModel.makeModelIndex(hovered.index)
        F.each(drop.urls, url => {
          store.tasksModel.requestOpenUrls(modelIndex, drop.urls)
        })
      }

      dragging = null
      hovered = null
    }
  }

  Component {
    id: horizontal
    RowLayout {
      spacing: 0

      FlexSpacing {
        fill: true
        smoothFill: true

        Layout.preferredWidth: store.state.geometry.panelNextPoint.x
      }
      Row {
        id: row
        spacing: store.state.icon.spacing
        layoutDirection: Qt.LeftToRight

        Layout.fillHeight: true

        Layout.topMargin: store.state.background.paddingY
        Layout.bottomMargin: store.state.background.paddingY

        Component.onCompleted: {
          panelContainer.positioner = row
        }

        move: Transition {
          enabled: animMoveEnabled
          NumberAnimation {
            property: 'x';
            easing.type: Easing.OutQuad
            duration: store.state.animation.duration / 4
            alwaysRunToEnd: true
          }
          NumberAnimation {
            property: 'y'
            to: 0
            easing.type: Easing.OutBounce
            duration: store.state.animation.duration
            alwaysRunToEnd: true
          }
        }
      }
      FlexSpacing {
        fill: true
      }
    }
  }

  Component {
    id: vertical
    ColumnLayout {
      spacing: 0
      
      FlexSpacing {
        fill: true
        smoothFill: true

        Layout.preferredHeight: store.state.geometry.panelNextPoint.y
      }
      Column {
        id: column
        spacing: store.state.icon.spacing

        Layout.fillWidth: true
        Layout.rightMargin: store.state.background.paddingY
        Layout.leftMargin: store.state.background.paddingY

        Component.onCompleted: {
          panelContainer.positioner = column
        }

        move: Transition {
          NumberAnimation {
            property: 'y';
            easing.type: Easing.OutQuad
            duration: store.state.animation.duration / 4
            alwaysRunToEnd: true
          }
          NumberAnimation {
            property: 'x'
            to: 0
            easing.type: Easing.OutBounce
            duration: store.state.animation.duration
            alwaysRunToEnd: true
          }
        }
      }
      FlexSpacing {
        fill: true
      }
    }
  }
}
