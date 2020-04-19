import QtQuick 2.12
import QtQuick.Layouts 1.1
import org.muelle.types 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import '../helpers/constants.ts' as Const
import '../../shared/functional.ts' as F
import '../Containers'
import '../../shared/components'
import QtGraphicalEffects 1.12

Item {
  id: icon
  objectName: 'IconTask'

  readonly property Item target: innerIcon
  property var m

  width: store.state.icon.size
  height: width

  DropShadow {
    source: innerIcon
    anchors.fill: innerIcon
    horizontalOffset: 0
    verticalOffset: 0
    radius: 4
    samples: 1 + radius * 2
    color: "#80000000" 
  }

  Box {
    id: innerIcon
    objectName: 'innerIcon'

    anchors.fill: icon
    paddings: store.state.icon.padding

    PlasmaCore.IconItem {
      id: iconItem
      source: m.decoration
      roundToIconSize: true
      usesPlasmaTheme: false
    }

    resources: [
      PaintItem { target: icon; enabled: false; showSize: false },
      PaintItem { target: iconItem; enabled: false; showSize: false }
    ]
  }

  Rectangle {
    id: indicator
    color: 
      m.IsActive 
        ? theme.highlightColor
        : m.IsDemandingAttention 
          ? "#FFDE3F"
          : theme.textColor
    
    visible: !m.IsLauncher

    width: 4
    height: 4
    radius: 2

    StateLayoutEdge {
      reset: AnchorChanges { 
        target: indicator
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
        target: indicator
        anchors {
          top: icon.top
          horizontalCenter: icon.horizontalCenter
        }
      }
      right: AnchorChanges {
        target: indicator
        anchors {
          right: icon.right
          verticalCenter: icon.verticalCenter
        }
      }
      bottom: AnchorChanges {
        target: indicator
        anchors {
          bottom: icon.bottom
          horizontalCenter: icon.horizontalCenter
        }
      }
      left: AnchorChanges {
        target: indicator
        anchors {
          left: icon.left
          verticalCenter: icon.verticalCenter
        }
      }
    }
  } 
}
