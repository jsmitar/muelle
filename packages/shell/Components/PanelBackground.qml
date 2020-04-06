import QtQuick 2.12
import org.muelle.extra 1.0 as Muelle
import org.muelle.types 1.0
import org.kde.plasma.core 2.0
import '../../shared/functional.ts' as F
import '../Components'

Item {
  id: background
  readonly property rect rect: store.state.geometry.backgroundRect

  x: rect.x
  y: rect.y
  width: rect.width
  height: rect.height

  EdgeSlideAnimation {
    target: frame
    distance: store.state.animation.edgeDistance
  }

  // Muelle.Rectangle {
  //   anchors.fill: parent
  //   color: theme.backgroundColor
  //   radius {
  //     topLeft: 4
  //     topRight: 4
  //   }
  // }

  FrameSvgItem {
    id: frame

    anchors.centerIn: background
    width: background.width
    height: background.height
    
    imagePath: 'widgets/background'
    enabledBorders: edgeToBorder[store.state.panel.edge]

    Binding {
      target: store.state.background
      property: 'paddingX'
      value: Math.max(frame.margins.left, frame.margins.right)
    }

    Binding {
      target: store.state.background
      property: 'paddingY'
      value: Math.max(frame.margins.top, frame.margins.bottom)
    }


    readonly property var edgeToBorder: ({ 
      [Types.Top]: FrameSvg.AllBorders ^ FrameSvg.TopBorder,
      [Types.Right]: FrameSvg.AllBorders ^ FrameSvg.RightBorder,
      [Types.Bottom]: FrameSvg.AllBorders ^ FrameSvg.BottomBorder,
      [Types.Left]: FrameSvg.AllBorders ^ FrameSvg.LeftBorder,
    })
  }

  // NOTE: Disabled for now
  // Behavior on width {
  //   NumberAnimation {
  //     easing.type: Easing.InOutQuad
  //     duration: store.state.animation.duration / 2
  //   }
  // }
  // Behavior on x {
  //   NumberAnimation {
  //     easing.type: Easing.InOutQuad
  //     duration: store.state.animation.duration / 2
  //   }
  // }
}