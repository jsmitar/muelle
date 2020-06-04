import QtQuick 2.14
import org.muelle.extra 1.0 as Muelle
import org.muelle.types 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import QtGraphicalEffects 1.12
import 'qrc:/shared/functional.ts' as F
import 'qrc:/shared/components'
import 'qrc:/shell/Components'

Item {
  id: background
  readonly property rect rect: store.state.geometry.backgroundRect
  readonly property string style: store.state.background.style
  readonly property int shadowBlur: store.state.background.shadowBlur
  readonly property Muelle.Radius bgradius: store.state.background.radius

  x: rect.x
  y: rect.y
  width: rect.width
  height: rect.height

  property bool enableAnim: !store.state.panel.updatingOrientation

  PaintItem {
    target: background
    enabled: false
  }

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

      Muelle.Rectangle {
        id: solid

        anchors.fill: parent
        
        color: theme.backgroundColor
        opacity: 0.8

        property var edgeToDegrees: ({
          [Types.Top]: 0,
          [Types.Right]: 270,
          [Types.Bottom]: 180,
          [Types.Left]: 90,
        })

        border {
          width: 0
          gradient {
            stops: ['#9FA4C4', '#B3CDD1']
            degrees: edgeToDegrees[store.state.panel.edge]
          }
        }
        gradient {
          stops: ['#33303e 1', '#232234 0']
          degrees: F.addDeg(edgeToDegrees[store.state.panel.edge], 0)
        }

        layer.enabled: true
        layer.effect: Glow {
          id: shadow

          anchors.fill: solid
          source: solid

          color: solid.color
          cached: false
          spread: 0.2
          samples: 2 * radius + 1
          radius: shadowBlur / 2
          transparentBorder: true
        }

        StateLayoutEdge {
          reset: PropertyChanges {
            target: solid
            radius {
              topLeft: 0
              topRight: 0
              bottomLeft: 0
              bottomRight: 0
            }
          }
          top: PropertyChanges {
            target: solid
            radius {
              topLeft: bgradius.bottomLeft
              topRight: bgradius.bottomRight
              bottomLeft: bgradius.topLeft
              bottomRight: bgradius.topRight
            }
          }
          right: PropertyChanges {
            target: solid
            radius {
              topLeft: bgradius.topLeft
              topRight: bgradius.bottomLeft
              bottomLeft: bgradius.topRight
              bottomRight: bgradius.bottomRight
            }
          }
          bottom: PropertyChanges {
            target: solid
            radius {
              topLeft: bgradius.topLeft
              topRight: bgradius.topRight
              bottomLeft: bgradius.bottomLeft
              bottomRight: bgradius.bottomRight
            }
          }
          left: PropertyChanges {
            target: solid
            radius {
              topLeft: bgradius.bottomLeft
              topRight: bgradius.topLeft
              bottomLeft: bgradius.bottomRight
              bottomRight: bgradius.topRight
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