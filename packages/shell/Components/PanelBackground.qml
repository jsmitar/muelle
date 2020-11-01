import QtQuick 2.14
import QtQuick.Window 2.14
import org.muelle.extra 1.0 as Muelle
import org.muelle.types 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import QtGraphicalEffects 1.12
import 'qrc:/shared/functional.ts' as F
import 'qrc:/shared/components'
import 'qrc:/shell/Components'

Item {
  id: background
  readonly property var geometry: store.state.geometry

  readonly property rect rect: geometry.background.next.rect
  readonly property string style: store.state.background.style
  readonly property bool maskGrowing: geometry.mask.growing
  readonly property int shadowBlur: store.state.background.shadowBlur
  readonly property Muelle.Radius bgradius: store.state.background.radius
  readonly property bool isAtStart: geometry.background.isAtStart
  readonly property bool isAtEnd: geometry.background.isAtEnd

  readonly property real noRadius: store.state.icon.size * 4

  x: rect.x
  y: rect.y
  width: rect.width
  height: rect.height

  property bool animEnabled: !store.state.panel.updatingOrientation

  NumberBehavior on x { enabled: $layout.isHorizontal && animEnabled }
  NumberBehavior on y { enabled: $layout.isVertical && animEnabled }
  NumberBehavior on width { enabled: $layout.isHorizontal && animEnabled }
  NumberBehavior on height { enabled: $layout.isVertical && animEnabled }

  Spy {
    properties: [ ]
  }

  Loader {
    id: frame

    sourceComponent: 
      style === 'solid' ? solidComp :
      style === 'svg'   ? svgComp   :
      undefined

    anchors.centerIn: parent
    width: parent.width
    height: parent.height

    EdgeSlideAnimation {
      id: slide
      target: frame
    }

    PaintItem {
      target: frame
      enabled: false
    }
  }

  Component {
    id: solidComp

    Item {
      id: content

      Muelle.ViewContrast {
        id: contrast
        view: $view

        contrastEnabled: false
        blurEnabled: false

        enabled:
          store.state.panel.visible && 
          !store.state.panel.updatingOrientation

        usePlasmaTheme: true
      }

      Muelle.Rectangle {
        id: solid

        width: background.width
        height: background.height
        visible: true
        
        color: '#202B2B2A'
        maskEnabled: true

        property var edgeToDegrees: ({
          [Types.Top]: 0,
          [Types.Right]: 270,
          [Types.Bottom]: 180,
          [Types.Left]: 90,
        })

        border {
          width: 0
          gradient {
            stops: ['#ff555555 0', '#ff333333 0.5']
            degrees: edgeToDegrees[store.state.panel.edge]
          }
        }
        gradient {
          stops: ['#EE263842 0']
          degrees: edgeToDegrees[store.state.panel.edge]
        }

        NumberBehavior on radius.topLeft {}
        NumberBehavior on radius.topRight {}
        NumberBehavior on radius.bottomLeft {}
        NumberBehavior on radius.bottomRight {}

        layer.enabled: true
        layer.effect: DropShadow {
          id: shadow
          source: solid
          width: solid.width
          height: solid.height

          color: '#50000000'
          cached: false
          spread: 0.3
          radius: shadowBlur
          samples: 1 + radius * 2
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
              topLeft: isAtStart ? noRadius : bgradius.bottomLeft
              topRight: isAtEnd ? noRadius : bgradius.bottomRight
              bottomLeft: isAtStart ? noRadius : bgradius.topLeft
              bottomRight: isAtEnd ? noRadius : bgradius.topRight
            }
          }
          right: PropertyChanges {
            target: solid
            radius {
              topLeft: isAtStart ? noRadius : bgradius.topLeft
              topRight: isAtStart ? noRadius : bgradius.bottomLeft
              bottomLeft: isAtEnd ? noRadius : bgradius.topRight
              bottomRight: isAtEnd ? noRadius : bgradius.bottomRight
            }
          }
          bottom: PropertyChanges {
            target: solid
            radius {
              topLeft: isAtStart ? noRadius : bgradius.topLeft
              topRight: isAtEnd ? noRadius : bgradius.topRight
              bottomLeft: isAtStart ? noRadius : bgradius.bottomLeft
              bottomRight: isAtEnd ? noRadius : bgradius.bottomRight
            }
          }
          left: PropertyChanges {
            target: solid
            radius {
              topLeft: isAtStart ? noRadius : bgradius.bottomLeft
              topRight: isAtStart ? noRadius : bgradius.topLeft
              bottomLeft: isAtEnd ? noRadius : bgradius.bottomRight
              bottomRight: isAtEnd ? noRadius : bgradius.topRight
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