import QtQuick 2.14

import org.kde.plasma.private.taskmanager 0.1 as TaskManagerApplet
import org.kde.plasma.core 2.0 as PlasmaCore
import org.muelle.extra 1.0 as Muelle
import org.muelle.types 1.0

import 'qrc:/shared/functional.ts' as F
import 'qrc:/shared/components'

QtObject {

  readonly property Component smartLauncher: Component {
    TaskManagerApplet.SmartLauncherItem {
      signal destruction
      Component.onDestruction: destruction()
    }
  }

  readonly property Component tooltip: Component {
    PlasmaCore.Dialog {
      outputOnly: true
      backgroundHints: PlasmaCore.Dialog.NoBackground
      type: PlasmaCore.Dialog.Tooltip
      flags: Qt.ToolTip

      signal destruction
      Component.onDestruction: destruction()

      property alias text: tooltipText.text
      
      readonly property var edgeToLocation: ({
        [Types.Top]: PlasmaCore.Types.TopEdge,
        [Types.Right]: PlasmaCore.Types.RightEdge,
        [Types.Bottom]: PlasmaCore.Types.BottomEdge,
        [Types.Left]: PlasmaCore.Types.LeftEdge
      })

      location: edgeToLocation[$layout.edge]
      mainItem: Box {
        id: box

        width: tooltipText.width + box.padding.x * 2
        height: tooltipText.height + box.padding.y * 2
        padding.x: 10
        padding.y: 10

        Muelle.Rectangle {
          id: bgTooltip

          radius {
            topLeft: 1.0
            topRight: 1.0
            bottomLeft: 1.0
            bottomRight: 1.0
          }

          property var edgeToDegrees: ({
            [Types.Top]: 0,
            [Types.Right]: 270,
            [Types.Bottom]: 180,
            [Types.Left]: 90,
          })

          gradient {
            stops: ['#33303e 1', '#232234 0']
            degrees: F.addDeg(edgeToDegrees[store.state.panel.edge], 0)
          }

          Text {
            id: tooltipText
            color: 'white'
            anchors.centerIn: parent
            topPadding: 4
            bottomPadding: 4
            leftPadding: 8
            rightPadding: 8
          }
        }
      }
    }
  }

  readonly property Component startup: Component { 
    QObject {
      id: _target

      property var start: rotationAnim.start

      property real rotation: 0

      property real duration: store.state.animation.shortDuration / 2
      
      signal destruction
      Component.onDestruction: destruction()

      SequentialAnimation {
        id: rotationAnim
        loops: 2

        RotationAnimation { 
          target: _target
          to: -5
          easing.type: Easing.OutSine
          duration: target.duration
        }
        RotationAnimation { 
          target: _target
          to: 0
          easing.type: Easing.InSine
          duration: target.duration / 2
        }
        RotationAnimation { 
          target: _target
          to: 5
          easing.type: Easing.OutSine
          duration: target.duration
        }
        RotationAnimation { 
          target: _target
          to: 0
          easing.type: Easing.InSine
          duration: target.duration / 2
        }
      }
    }
  }
}