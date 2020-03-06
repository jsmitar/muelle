import QtQuick 2.12
import QtQuick.Window 2.12
import org.muelle.types 1.0
import '../../../shared/components'

QObject {
  objectName: '@State/Geometry'

  property var state

  property bool composition: true

  readonly property alias viewSize: view.size

  readonly property alias backgroundRect: background.rect

  property alias panelOffset: panel.offset

  readonly property alias panelSize: panel.size
  readonly property alias panelRect: panel.rect

  readonly property alias panelNextRect: next.rect
  readonly property alias panelNextSize: next.size
  readonly property alias panelNextPoint: next.point

  readonly property alias maskRect: mask.rect
  property alias maskGrowing: mask.growing
  property alias maskEnabled: mask.enabled

  property bool lockMaskGrowing: false


  readonly property bool _isHorizontal: state.panel.isHorizontal

  readonly property bool _isTopOrLeftEdge:
    state.panel.edge === Types.Top || state.panel.edge === Types.Left 

  /*
   * the approach for the calculation is calc all sizes(width x height) as a 
   * horizontal panel.
   * the strategy is the same for the point(topLeft x,y), calc, the values for 
   * Edge::Top, Edge::Left panel
   * The last step is turn the values sizes,point for create the geometries
   */

  QObject {
    id: view
    objectName: 'view'

    readonly property int width: 
      _isHorizontal ? Screen.width : Screen.height

    readonly property int height:
      Math.max(background.height, panel.height)

    readonly property size size: state.panel.isHorizontal
      ? Qt.size(width, height)
      : Qt.size(height, width)

  }

  QObject {
    id: background
    objectName: 'background'

    readonly property int paddingX: 30

    readonly property int paddingY: 10

    readonly property int x: Math.max(panel.next.x - paddingX, 0)

    readonly property int y: _isTopOrLeftEdge
      ? 0
      : view.height - height

    readonly property int width: panel.next.width + paddingX * 2

    readonly property int maxHeight: state.icon.size + paddingY

    readonly property int height: Math.min(state.background.lift, maxHeight)

    readonly property rect rect: _isHorizontal
      ? Qt.rect(x, y, width, height)
      : Qt.rect(y, x, height, width)
  }

  QObject {
    id: panel
    objectName: 'panel'

    property real offset: 0

    readonly property int xCenter: (view.width - width) / 2

    readonly property int x: xCenter + offset * xCenter

    readonly property int y: _isTopOrLeftEdge
      ? 0 : Math.max(0, view.height - height)

    readonly property int width:
      state.icon.size * state.panel.taskCount1 +
      state.icon.spacing * Math.max(0, state.panel.taskCount1 - 1)

    readonly property int height: state.icon.size

    readonly property size size: _isHorizontal
      ? Qt.size(width, height)
      : Qt.size(height, width)

    readonly property rect rect: _isHorizontal
      ? Qt.rect(x, y, width, height)
      : Qt.rect(y, x, height, width)

    readonly property QObject next: QObject {
      id: next
      objectName: 'next'

      readonly property int xCenter: (view.width - width) / 2

      readonly property int x: xCenter + panel.offset * xCenter

      readonly property int y: _isTopOrLeftEdge
        ? 0 : Math.max(0, view.height - height)

      readonly property int width:
        state.icon.size * state.panel.nextTaskCount1 +
        state.icon.spacing * Math.max(0, state.panel.nextTaskCount1 - 1)

      readonly property int height: state.icon.size

      readonly property size size: _isHorizontal
        ? Qt.size(width, height)
        : Qt.size(height, width)

      readonly property point point: _isHorizontal
        ? Qt.point(x, y)
        : Qt.point(y, x) 
      
      readonly property rect rect: _isHorizontal
        ? Qt.rect(x, y, width, height)
        : Qt.rect(y, x, height, width)
    }
  }

  QObject {
    id: mask
    objectName: 'mask'

    property bool growing: false

    property bool enabled: true

    QtObject {
      id: visible

      readonly property int gap: store.state.icon.size * 2

      readonly property rect rectHorizontal: mask.growing
        ? Qt.rect(panel.next.x - gap / 2, 0, panel.next.width + gap, panel.next.height)
        : Qt.rect(background.x, 0, background.width, view.height)

      readonly property rect rectVertical: mask.growing
        ? Qt.rect(0, panel.next.x - gap / 2, view.height, panel.next.width + gap)
        : Qt.rect(0, background.x, view.height, background.width)

      readonly property rect rect: _isHorizontal ? rectHorizontal : rectVertical
    }

    QtObject {
      id: hidden

      readonly property rect rect: Qt.rect(-1, -1, 1, 1)
    }

    readonly property rect noMask: Qt.rect(0, 0, 0, 0)

    readonly property rect rect: enabled 
      ? state.panel.visible ? visible.rect : hidden.rect
      : noMask
  }

  // Spy {
  //   delayed: true
  //   // properties: [
  //   //   spy`${view}.size`,
  //   //   spy`${panel}.rect,nextRect`,
  //   //   spy`${mask}.rect`
  //   // ]
  // }
}
