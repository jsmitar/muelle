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

  Drag.active: !!(dragging && dragging.drag.active && dragging.hold)
  Drag.hotSpot.x: store.state.icon.size / 2
  Drag.hotSpot.y: store.state.icon.size / 2
  Drag.dragType: Drag.Automatic

  Repeater { parent: positioner; model: panelContainer.model }

  DropArea {
    id: dropArea

    readonly property rect rect: store.state.geometry.panelNextRect

    anchors {
      fill: parent
      leftMargin: rect.x
      rightMargin: parent.width - rect.width - rect.x
      topMargin: rect.y
      bottomMargin: parent.height - rect.height - rect.y
    }

    function moveTask() {
      if (dragging && hovered) {

        store.tasksModel.move(
          dragging.DelegateModel.itemsIndex,
          hovered.DelegateModel.itemsIndex
        )
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
      store.dispatch(Action.syncLaunchers())
    }
  }

  Component {
    id: horizontal
    RowLayout {
      spacing: 0

      FlexSpacing {
        fill: true
        smoothFill: store.state.geometry.maskEnabled

        Layout.preferredWidth: store.state.geometry.panelNextPoint.x
      }
      Row {
        id: row
        spacing: store.state.icon.spacing
        layoutDirection: Qt.LeftToRight
        Layout.fillHeight: true

        Component.onCompleted: {
          panelContainer.positioner = row
        }

        move: Transition {
          NumberAnimation {
            alwaysRunToEnd: true
            properties: 'x';
            easing.type: Easing.InOutQuad
            duration: store.state.animation.duration / 2
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
        smoothFill: store.state.geometry.maskEnabled

        Layout.preferredHeight: store.state.geometry.panelNextPoint.y
      }
      Column {
        id: column
        spacing: store.state.icon.spacing
        Layout.fillWidth: true

        Component.onCompleted: {
          panelContainer.positioner = column
        }

        move: Transition {
          NumberAnimation {
            alwaysRunToEnd: true
            properties: 'y';
            easing.type: Easing.InOutQuad
            duration: store.state.animation.duration / 2
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
