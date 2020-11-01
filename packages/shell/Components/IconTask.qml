import QtQuick 2.14
import QtQuick.Layouts 1.1
import org.kde.plasma.core 2.0 as PlasmaCore
import org.muelle.extra 1.0 as Muelle
import org.muelle.types 1.0
import QtGraphicalEffects 1.12

import 'qrc:/shell/helpers/constants.ts' as Const
import 'qrc:/shared/functional.ts' as F
import 'qrc:/shell/Containers'
import 'qrc:/shell/Components'
import 'qrc:/shared/components'

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

  SequentialAnimation {
    running: !hasLauncher
    ScriptAction {
      script: innerIcon.visible = false
    }
    PauseAnimation {
      duration: store.state.animation.duration / 4
    }
    ScriptAction {
      script: innerIcon.visible = true
    }
    ParallelAnimation {
      OpacityAnimator {
        target: icon
        from: 0
        to: 1
        duration: store.state.animation.duration
      }
      NumberAnimation {
        target: innerIcon.margin
        property: $layout.isVertical ? 'x' : 'y'
        from: 
          icon.size *
          ($layout.layout & (Types.Left | Types.Top) ? -1 : 1) 
        to: 0 
        easing.type: Easing.OutQuad
        duration: store.state.animation.duration / 2
      }
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
      antialiasing: true
      smooth: true
      z: 1
    }

    // DropShadow {
    //   transparentBorder: true
    //   source: iconItem
    //   width: iconItem.width
    //   height: iconItem.height
    //   horizontalOffset: 0
    //   verticalOffset: 0
    //   radius: 8
    //   samples: 1 + radius * 2
    //   color: "#80000000" 
    //   z: 0
    // }

    resources: [
      PaintItem { target: icon; enabled: false; showSize: false },
      PaintItem { target: iconItem; enabled: false; showSize: false }
    ]
  }

  TaskGroup {
    id: tooltip
    m: icon.m
    visualParent: icon
    hover: isStartup || store.state.panel.slide !== 'none' 
      ? false 
      : icon.hover
  }

  StartupAnimation {
    id: startupAnimation
    objectName: m.AppName
    running: isStartup
    target: innerIcon
  }

  SmartLauncher {
    id: smartLauncher
    objectName: m.AppName
    launcherUrl: m.LauncherUrlWithoutIcon
  }

  TaskIndicator {
    id: indicator
    visible: innerIcon.visible
    isActive: !!m.IsActive
    isGroupParent: !!m.IsGroupParent
    isLauncher: !!m.IsLauncher
    isDemandingAttention: !!m.IsDemandingAttention
  }

  Badge {
    id: badge
    visible: smartLauncher.countVisible
    label: smartLauncher.count

    // Component.onCompleted: {
    //   // if (m.index == 1) {
    //   setInterval(() => {
    //     badge.visible = true
    //     badge.label = +badge.label + 1
    //     console.log('HOLA')
    //   }, 1000)
    //   // }
    // }
    
    anchors {
      right: icon.right
      top: icon.top
      rightMargin: innerIcon.paddings + 2
      topMargin: innerIcon.paddings + 2
    }
  }

  ProgressBar {
    id: progress
    active: smartLauncher.progressVisible
    value: smartLauncher.progress / 100
    height: 7

    anchors {
      left: icon.left
      right: icon.right
      bottom: icon.bottom
      margins: 5
      bottomMargin: 8
    }
  }

}
