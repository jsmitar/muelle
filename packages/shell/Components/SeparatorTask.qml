import QtQuick 2.14
import QtQuick.Layouts 1.1
import org.muelle.types 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import 'qrc:/shell/helpers/constants.ts' as Const
import 'qrc:/shared/functional.ts' as F
import 'qrc:/shell/Containers'
import 'qrc:/shared/components'
import QtGraphicalEffects 1.12

Box {
  id: separator
  objectName: 'SeparatorTask'

  property var m
  readonly property Item target: separator.content[0]

  resources: [
    StateLayoutOrientation {
      horizontal: PropertyChanges {
        target: separator
        width: store.state.icon.separator
        height: store.state.icon.size
        padding.x: store.state.icon.padding
        padding.y: store.state.icon.padding * 3
      }
      vertical: PropertyChanges {
        target: separator
        width: store.state.icon.size
        height: store.state.icon.separator
        padding.x: store.state.icon.padding * 3
        padding.y: store.state.icon.padding
      }
    }
  ]

  Rectangle {
    border.width: 0
    gradient: Gradient {
      orientation: store.state.panel.isVertical
        ? Gradient.Horizontal 
        : Gradient.Vertical
      
      GradientStop{ position: 0.0; color: "transparent" }
      GradientStop{ position: 0.2; color: "grey" }
      GradientStop{ position: 0.8; color: "grey" }
      GradientStop{ position: 1.0; color: "transparent" }
    }
  }
}
