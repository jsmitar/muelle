import QtQuick 2.12
import QtQuick.Window 2.12
import org.muelle.types 1.0
import '../../../shared/components'

QObject {
  objectName: '@State/Geometry'

  property var state

  property bool composition: true

  readonly property alias viewSize: view.size

  property alias panelOffset: panel.offset

  readonly property alias panelSize: panel.size
  readonly property alias panelRect: panel.rect

  readonly property alias panelNextRect: panel.nextRect
  readonly property alias panelNextSize: panel.nextSize
  readonly property alias panelNextPoint: panel.nextPoint
  readonly property alias panelPointCenter: panel.pointCenter

  readonly property alias maskRect: mask.rect
  property alias maskGrowing: mask.growing
  property alias maskEnabled: mask.enabled

  property bool lockMaskGrowing: false

  QObject {
    id: view
    objectName: 'view'

    readonly property int width:
        state.panel.isHorizontal ? Screen.width : Screen.height

    readonly property int height: panel.height

    readonly property size size: state.panel.isHorizontal
      ? Qt.size(Screen.width, height)
      : Qt.size(height, Screen.height)
  }


  QObject {
    id: panel
    objectName: 'panel'

    property real offset: 0

    readonly property int xCenter: (view.width - width) / 2

    readonly property int xNextCenter: (view.width - nextWidth) / 2

    readonly property int x: xCenter + offset * xCenter

    readonly property int y: 0

    readonly property int width:
      state.icon.size * state.panel.taskCount1 +
      state.icon.spacing * Math.max(0, state.panel.taskCount1 - 1)

    readonly property int height: state.icon.size

    readonly property int nextX: xNextCenter + offset * xNextCenter

    readonly property int nextY: 0

    readonly property int nextWidth:
      state.icon.size * state.panel.nextTaskCount1 +
      state.icon.spacing * Math.max(0, state.panel.nextTaskCount1 - 1) 

    readonly property int nextHeight: state.icon.size

    readonly property size size: state.panel.isHorizontal
      ? Qt.size(width, height)
      : Qt.size(height, width)

    readonly property rect rect: state.panel.isHorizontal
      ? Qt.rect(x, y, width, height)
      : Qt.rect(y, x, height, width)

    readonly property rect nextRect: state.panel.isHorizontal
      ? Qt.rect(nextX, nextY, nextWidth, nextHeight)
      : Qt.rect(nextY, nextX, nextHeight, nextWidth)

    readonly property size nextSize: state.panel.isHorizontal
      ? Qt.size(nextWidth, nextHeight)
      : Qt.size(nextHeight, nextWidth)

    readonly property point nextPoint: state.panel.isHorizontal
      ? Qt.point(nextX, nextY)
      : Qt.point(nextY, nextX)

    readonly property point pointCenter: state.panel.isHorizontal
      ? Qt.point(xCenter, y)
      : Qt.point(y, xCenter)
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
        ? Qt.rect(panel.nextX - gap / 2, panel.nextY, panel.nextWidth + gap, panel.nextHeight)
        : Qt.rect(panel.x, panel.y, panel.width, panel.height)

      readonly property rect rectVertical: mask.growing
        ? Qt.rect(panel.nextY, panel.nextX - gap / 2, panel.nextHeight, panel.nextWidth + gap)
        : Qt.rect(panel.y, panel.x, panel.height, panel.width)

      readonly property rect rect: state.panel.isHorizontal ? rectHorizontal : rectVertical
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

  Spy {
    delayed: true
    // properties: [
    //   spy`${view}.size`,
    //   spy`${panel}.rect,nextRect`,
    //   spy`${mask}.rect`
    // ]
  }
}
