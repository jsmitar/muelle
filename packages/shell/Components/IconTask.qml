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
  property var m: ({})
  property bool hover: false

  readonly property int size: store.state.icon.size

  property bool isStartup: !!m.IsStartup
  property bool hasLauncher: !!m.HasLauncher

  width: size
  height: size

  DropShadow {
    source: innerIcon
    anchors.fill: innerIcon
    horizontalOffset: 0
    verticalOffset: 0
    radius: 4
    samples: 1 + radius * 2
    color: "#80000000" 
  }

  SequentialAnimation {
    running: !hasLauncher
    
    NumberAnimation {
      target: innerIcon.margin
      property: $layout.isVertical ? 'x' : 'y'
      from: 
        store.state.animation.edgeDistance *
        ($layout.layout & (Types.Left | Types.Top) ? -1 : 1) 
      to: 0 
      easing.type: Easing.OutQuad
      duration: store.state.animation.duration / 2
    }
  }

  Box {
    id: innerIcon
    objectName: 'innerIcon'

    anchors.centerIn: icon
    width: size
    height: size

    paddings: store.state.icon.padding

    PlasmaCore.IconItem {
      id: iconItem
      source: m.decoration || 'application-default-icon'
      roundToIconSize: false
      usesPlasmaTheme: false
      active: hover
      animated: false
    }

    resources: [
      PaintItem { target: icon; enabled: false; showSize: false },
      PaintItem { target: iconItem; enabled: false; showSize: false }
    ]
  }

  StartupAnimation {
    id: startupAnimation
    objectName: m.AppName
    running: isStartup
    target: iconItem
  }

  SmartLauncher {
    id: smartLauncher
    objectName: m.AppName
    launcherUrl: m.LauncherUrlWithoutIcon
  }

  Badge {
    visible: smartLauncher.countVisible
    label: smartLauncher.count
    
    anchors {
      right: icon.right
      bottom: icon.bottom
      rightMargin: innerIcon.paddings + 2
      bottomMargin: innerIcon.paddings + 2
    }
  }

  Item {
    id: indicator

    implicitWidth: 4
    implicitHeight: 4

    ProgressBar {
      id: progress
      active: smartLauncher.progressVisible
      value: smartLauncher.progress / 100

      anchors.fill: indicator
    }

    StateLayoutEdge {
      reset: AnchorChanges { 
        target: indicator
        anchors {
          top: undefined
          right: undefined
          bottom: undefined
          left: undefined
        }
      }
      top: AnchorChanges {
        target: indicator
        anchors {
          top: icon.top
          left: icon.left
          right: icon.right
        }
      }
      right: AnchorChanges {
        target: indicator
        anchors {
          top: icon.top
          right: icon.right
          bottom: icon.bottom
        }
      }
      bottom: AnchorChanges {
        target: indicator
        anchors {
          left: icon.left
          right: icon.right
          bottom: icon.bottom
        }
      }
      left: AnchorChanges {
        target: indicator
        anchors {
          top: icon.top
          left: icon.left
          bottom: icon.bottom
        }
      }
    }
  }

  Rectangle {
    id: indicator_
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

    
  } 
}
