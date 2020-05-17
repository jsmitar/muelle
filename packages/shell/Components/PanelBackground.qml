import QtQuick 2.12
import org.muelle.extra 1.0 as Muelle
import org.muelle.types 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import QtGraphicalEffects 1.12
import '../../shared/functional.ts' as F
import '../../shared/components'
import '../Components'

Item {
  id: background
  readonly property rect rect: store.state.geometry.backgroundRect
  readonly property string style: store.state.background.style

  x: rect.x
  y: rect.y
  width: rect.width
  height: rect.height

  property bool enableAnim: !store.state.panel.updatingOrientation

  Behavior on x {
    enabled: $layout.isHorizontal && enableAnim 
    NumberAnimation {
      easing.type: Easing.OutQuad
      duration: store.state.animation.duration / 2
    }
  }

  Behavior on y {
    enabled: $layout.isVertical && enableAnim 
    NumberAnimation {
      easing.type: Easing.OutQuad
      duration: store.state.animation.duration / 2
    }
  }

  Behavior on width {
    enabled: $layout.isHorizontal && enableAnim 
    NumberAnimation {
      easing.type: Easing.OutQuad
      duration: store.state.animation.duration / 2
    }
  }

  Behavior on height {
    enabled: $layout.isVertical && enableAnim 
    NumberAnimation {
      easing.type: Easing.OutQuad
      duration: store.state.animation.duration / 2
    }
  }

  EdgeSlideAnimation {
    target: frame
  }

  Item {
    id: contrast_sync
    visible: false

    anchors.centerIn: background
    width: background.width
    height: background.height

    EdgeSlideAnimation {
      target: contrast_sync
      delay: 0
    }
  }

  Loader {
    id: frame

    sourceComponent: 
      style === 'solid' ? solidComp :
      style === 'svg'   ? svgComp   :
      undefined

    anchors.centerIn: background
    width: background.width
    height: background.height
  }

  Component {
    id: solidComp

    Item {
      id: content

      readonly property int shadowBlur: store.state.background.shadowBlur
      readonly property int borderRadius: store.state.background.borderRadius
      readonly property int paddingX: store.state.background.paddingX
      readonly property int paddingY: store.state.background.paddingY

      Binding {
        target: store.state.background
        property: 'paddingX'
        value: content.shadowBlur * 3
      }

      Binding {
        target: store.state.background
        property: 'paddingY'
        value: content.shadowBlur * 3
      }

      RectangularGlow {
        id: shadow

        anchors.fill: solid

        cached: false
        color: theme.backgroundColor
        spread: 0.2
        glowRadius: shadowBlur
        cornerRadius: Math.min(width, height) / 2.0 + glowRadius
      }

      Muelle.Rectangle {
        id: solid

        anchors.fill: parent
        
        color: theme.backgroundColor

        StateLayoutEdge {
          reset: PropertyChanges {
            target: solid
            radius {
              topLeft: 0
              topRight: 0
              bottomLeft: 0
              bottomRight: 0
            }
            anchors {
              topMargin: 0
              rightMargin: 0
              bottomMargin: 0
              leftMargin: 0
            }
          }
          top: PropertyChanges {
            target: solid
            radius {
              bottomLeft: borderRadius
              bottomRight: borderRadius
            }
            anchors {
              leftMargin: paddingX
              rightMargin: paddingX
              bottomMargin: paddingY
            }
          }
          right: PropertyChanges {
            target: solid
            radius {
              topLeft: borderRadius
              bottomLeft: borderRadius
            }
            anchors {
              topMargin: paddingY
              bottomMargin: paddingY
              leftMargin: paddingX
            }
          }
          bottom: PropertyChanges {
            target: solid
            radius {
              topLeft: borderRadius
              topRight: borderRadius
            }
            anchors {
              leftMargin: paddingX
              rightMargin: paddingX
              topMargin: paddingY
            }
          }
          left: PropertyChanges {
            target: solid
            radius {
              topRight: borderRadius
              bottomRight: borderRadius
            }
            anchors {
              topMargin: paddingY
              bottomMargin: paddingY
              rightMargin: paddingX
            }
          }
        }
      }
    }
  }

  Component {
    id: svgComp

    Item {
      readonly property var edgeToBorder: ({ 
        [Types.Top]: PlasmaCore.FrameSvg.AllBorders ^ PlasmaCore.FrameSvg.TopBorder,
        [Types.Right]: PlasmaCore.FrameSvg.AllBorders ^ PlasmaCore.FrameSvg.RightBorder,
        [Types.Bottom]: PlasmaCore.FrameSvg.AllBorders ^ PlasmaCore.FrameSvg.BottomBorder,
        [Types.Left]: PlasmaCore.FrameSvg.AllBorders ^ PlasmaCore.FrameSvg.LeftBorder,
      })

      readonly property var edgeToCardinal: ({
        [Types.Top]: 'north',
        [Types.Right]: 'east',
        [Types.Bottom]: 'south',
        [Types.Left]: 'west',
      })

      Binding {
        target: store.state.background
        property: 'paddingX'
        value: Math.max(
          bgSvg.margins.left, 
          bgSvg.margins.right
        )
      }

      Binding {
        target: store.state.background
        property: 'paddingY'
        value: Math.max(
          bgSvg.margins.top, 
          bgSvg.margins.bottom
        )
      }

      Binding {
        target: store.state.background
        property: 'shadowBlur'
        value: Math.max(
          shadow.margins.top,
          shadow.margins.right,
          shadow.margins.bottom,
          shadow.margins.left
        )
      }

      Muelle.ViewContrast {
        id: contrast
        view: $view

        mask: bgSvg.mask
        position: Qt.point(
          background.x + contrast_sync.x,
          background.y + contrast_sync.y
        )

        contrastEnabled: false
        blurEnabled: false

        enabled:
          store.state.panel.visible && 
          !store.state.panel.updatingOffset &&
          !store.state.panel.updatingOrientation

        usePlasmaTheme: true
      }

      PlasmaCore.FrameSvgItem {
        id: shadow

        anchors { 
          fill: parent
          margins: -store.state.background.shadowBlur
        }

        imagePath: 'widgets/panel-background'
        enabledBorders: edgeToBorder[store.state.panel.edge]
        prefix: 'shadow'
      }

      PlasmaCore.FrameSvgItem {
        id: bgSvg
        
        anchors { 
          fill: parent
        }
      
        imagePath: 'widgets/panel-background'
        enabledBorders: edgeToBorder[store.state.panel.edge]
        prefix: [edgeToCardinal[store.state.panel.edge], '']
        colorGroup: PlasmaCore.Theme.NormalColorGroup
      }
    }
  }
}