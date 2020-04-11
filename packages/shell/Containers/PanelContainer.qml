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

  Drag.active: dragging && dragging.drag.active
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

    property var syncLaunchers: F.debounce(store.tasksModel.syncLaunchers, 500)

    function moveTask() {
      if (dragging && hovered) {

        store.tasksModel.move(
          dragging.DelegateModel.itemsIndex,
          hovered.DelegateModel.itemsIndex
        )
        syncLaunchers()
      }
    }

    drag.onPositionChanged: {
      const item = positioner.childAt(drag.x, drag.y)
      if (item !== hovered) {
        hovered = item
        moveTask()
      }
    }

    onEntered: {
      const apps = Seq
        .from(drag.urls)
        .filter(url => store.backend.isApplication(url))
        .each(url => store.backend.addLauncher(url))
      
      if (apps.length) {
        drag.accept()
      }
    }

    onDropped: {
      const urls = Seq
        .from(drop.urls)
        .filter(url => !store.backend.isApplication(url))
        .value

      if (urls && hovered) {
        const modelIndex = store.tasksModel.makeModelIndex(hovered.index)
        store.tasksModel.requestOpenUrls(modelIndex, drop.urls)
      }
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
          NumberAnimation {
            property: 'x';
            easing.type: Easing.OutQuad
            duration: store.state.animation.duration / 2
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
        add: Transition {
          enabled: store.state.animation.addEnabled
          NumberAnimation {
            property: 'y'
            from: ($layout.edge === Types.Top ? -1 : 1) * store.state.icon.size
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
            duration: store.state.animation.duration / 2
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
        add: Transition {
          enabled: store.state.animation.addEnabled
          NumberAnimation {
            property: 'x'
            from: ($layout.edge === Types.Left ? -1 : 1) * store.state.icon.size
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
