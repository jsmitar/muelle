import QtQuick 2.14
import QtQuick.Layouts 1.3
import QtQml.Models 2.3
import org.muelle.types 1.0

import 'qrc:/shell/Store/actions.ts' as Action
import 'qrc:/shared/components'
import 'qrc:/shell/Components'
import 'qrc:/shared/functional.ts' as F

Loader {
  id: panelContainer
  objectName: 'PanelContainer'

  asynchronous: false
  sourceComponent: $layout.isHorizontal ? horizontal : vertical

  property alias hovered: dragAndDrop.hovered
  property alias dragging: dragAndDrop.dragging
  property alias removedLauncher: dragAndDrop.removedLauncher

  default property DelegateModel model
  property Item positioner

  property bool syncLaunchers: false

  DragAndDropHandler {
    id: dragAndDrop
    target: panelContainer
  }
  PaintItem {
    target: positioner
    enabled: false
  }

  Drag.active: false
  Drag.hotSpot.x: dragging ? dragging.width / 2 : 0
  Drag.hotSpot.y: dragging ? dragging.height / 2 : 0
  Drag.dragType: Drag.Automatic

  Repeater { parent: positioner; model: panelContainer.model }

  Component {
    id: horizontal
    RowLayout {
      spacing: 0

      FlexSpacing {
        fill: true
        smoothFill: true

        Layout.preferredWidth: store.state.geometry.panel.next.point.x
      }
      Row {
        id: row
        spacing: store.state.icon.spacing
        layoutDirection: Qt.LeftToRight

        Layout.alignment: Qt.AlignTop
        Layout.topMargin: store.state.geometry.panel.point.y
        Layout.preferredHeight: store.state.geometry.panel.size.height 

        Component.onCompleted: {
          panelContainer.positioner = row
        }

        move: Transition {
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

        Layout.preferredHeight: store.state.geometry.panel.next.point.y
      }
      Column {
        id: column
        spacing: store.state.icon.spacing

        Layout.alignment: Qt.AlignLeft
        Layout.leftMargin: store.state.geometry.panel.point.x
        Layout.preferredWidth: store.state.geometry.panel.size.width

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
