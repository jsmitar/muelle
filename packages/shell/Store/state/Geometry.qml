import QtQuick 2.14
import QtQuick.Window 2.12
import org.muelle.types 1.0
import 'qrc:/shared/components'

QObject {
  objectName: '@State/Geometry'

  property var state

  property bool composition: true

  readonly property alias viewSize: view.size

  readonly property alias backgroundRect: background.rect
  readonly property alias backgroundPoint: background.point
  readonly property alias backgroundInset: background.inset

  property alias panelOffset: panel.offset

  readonly property alias panelSize: panel.size
  readonly property alias panelRect: panel.rect

  readonly property alias panelNextRect: next.rect
  readonly property alias panelNextSize: next.size
  readonly property alias panelNextPoint: next.point

  readonly property alias maskRect: mask.rect
  property alias maskGrowing: mask.growing
  property alias maskEnabled: mask.enabled
  property alias maskFull: mask.full

  readonly property bool _isHorizontal: state.panel.isHorizontal

  readonly property bool _isTopOrLeftEdge:
    state.panel.edge === Types.Top || state.panel.edge === Types.Left 

  /*
   * the approach for the calculation is calc all sizes(width x height) as a 
   * horizontal panel or bottom panel.
   * the strategy is the same for the point(topLeft x,y) calc, the values for 
   * Edge::Top, Edge::Left panel
   * The last step is turn the values sizes,point for create the geometries
   */

  readonly property QObject view: QObject {
    id: view
    objectName: 'view'

    readonly property int width: 
      _isHorizontal ? Screen.width : Screen.height

    readonly property int height:
      Math.max(
        background.boxHeight, 
        panel.height
      ) 

    readonly property size size: state.panel.isHorizontal
      ? Qt.size(width, height)
      : Qt.size(height, width)
  }

  readonly property QObject background: QObject {
    id: background
    objectName: 'background'

    readonly property int shadow: state.background.shadowBlur

    readonly property int paddingX: state.icon.size * state.background.paddingX

    readonly property int paddingTop: state.icon.size * state.background.paddingTop

    readonly property int paddingBottom: state.icon.size * state.background.paddingBottom

    readonly property int inset: state.background.inset + paddingBottom

    readonly property int x: panel.next.x - paddingX

    readonly property int y: _isTopOrLeftEdge
      ? inset
      : view.height - height - state.background.inset

    readonly property int width:
      paddingX * 2 + panel.next.width

    readonly property int height: 
      paddingTop + state.icon.size + paddingBottom

    readonly property int boxHeight:
      paddingTop + state.icon.size + inset + shadow

    readonly property point point: _isHorizontal 
      ? Qt.point(x, y)
      : Qt.point(y, x)

    readonly property rect rect: _isHorizontal
      ? Qt.rect(x, y, width, height)
      : Qt.rect(y, x, height, width)
  }

  readonly property QObject panel: QObject {
    id: panel
    objectName: 'panel'

    property real offset: 0

    readonly property int xCenter: (view.width - width) / 2

    readonly property int x: xCenter + offset * xCenter

    readonly property int y: _isTopOrLeftEdge
      ? 0
      : Math.max(0, view.height - height)

    readonly property int width:
      state.icon.size * state.panel.taskCount1 +
      state.icon.separator * state.panel.separatorCount +
      state.icon.spacing * Math.max(0, state.panel.taskCount1 + state.panel.separatorCount - 1)

    readonly property int height: state.icon.size + background.inset

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
        ? 0 
        : Math.max(0, view.height - height)

      readonly property int width:
        state.icon.size * state.panel.nextTaskCount1 +
        state.icon.separator * state.panel.separatorCount +
        state.icon.spacing * Math.max(0, state.panel.nextTaskCount1 + state.panel.separatorCount - 1)

      readonly property int height: state.icon.size + background.inset

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

  readonly property QObject mask: QObject {
    id: mask
    objectName: 'mask'

    property bool growing: false

    property bool full: 
      state.panel.updatingOffset ||
      state.panel.updatingOrientation

    property bool enabled: true

    QtObject {
      id: visible

      readonly property int gap: mask.growing 
        ? store.state.icon.size * 2 + background.shadow * 2 + background.paddingX * 2
        : background.shadow * 2

      readonly property rect rectHorizontal: mask.growing
        ? Qt.rect(panel.next.x - gap / 2, 0, panel.next.width + gap, view.height)
        : Qt.rect(background.x - gap / 2, 0, background.width + gap, view.height)

      readonly property rect rectVertical: mask.growing
        ? Qt.rect(0, panel.next.x - gap / 2, view.height, panel.next.width + gap)
        : Qt.rect(0, background.x - gap / 2, view.height, background.width + gap)

      readonly property rect rect: _isHorizontal ? rectHorizontal : rectVertical
    }

    QtObject {
      id: hidden

      readonly property rect rect: Qt.rect(-1, -1, 1, 1)
    }

    readonly property rect noMask: Qt.rect(0, 0, 0, 0)

    readonly property rect rect: 
      full ? noMask 
           : enabled 
              ? state.panel.visible ? visible.rect 
                                    : hidden.rect
              : noMask
  }
}
