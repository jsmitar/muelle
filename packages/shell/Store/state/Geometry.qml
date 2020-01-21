import QtQuick 2.12
import QtQuick.Window 2.12
import org.muelle.types 1.0
import '../../../shared/components'

QObject {
  objectName: '@State/Geometry'

  property var state

  property bool composition: true

  readonly property alias viewSize: view.size

  readonly property alias panelRect: panel.rect

  readonly property alias panelNextRect: panel.nextRect

  readonly property alias maskRect: mask.rect

  readonly property alias panelNextPoint: panel.nextPoint

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

    readonly property int x: (view.width - width) / 2

    readonly property int y: 0

    readonly property int width:
      state.icon.size * state.panel.taskCount1 +
      state.icon.spacing * Math.max(0, state.panel.taskCount1 - 1)

    readonly property int height: state.icon.size

    readonly property int nextX: (view.width - nextWidth) / 2

    readonly property int nextY: 0

    readonly property int nextWidth:
      state.icon.size * state.panel.nextTaskCount1 +
      state.icon.spacing * Math.max(0, state.panel.nextTaskCount1 - 1)

    readonly property int nextHeight: state.icon.size

    readonly property rect rect: state.panel.isHorizontal
      ? Qt.rect(x, y, width, height)
      : Qt.rect(y, x, height, width)

    readonly property rect nextRect: state.panel.isHorizontal
      ? Qt.rect(nextX, nextY, nextWidth, nextHeight)
      : Qt.rect(nextY, nextX, nextHeight, nextWidth)

    readonly property point nextPoint: state.panel.isHorizontal
      ? Qt.point(nextX, nextY)
      : Qt.point(nextY, nextX)

  }

  QObject {
    id: mask
    objectName: 'mask'

    QtObject {
      id: visible

      readonly property bool grow:
          state.panel.nextTaskCount1 >= state.panel.taskCount1

      readonly property rect rect: grow
        ? Qt.rect(panel.nextX, panel.nextY, panel.nextWidth, panel.nextHeight)
        : Qt.rect(panel.x, panel.y, panel.width, panel.height)
    }

    QtObject {
      id: hidden

      readonly property rect rect: Qt.rect(0, 0, 1, 1)
    }

    readonly property rect rect: state.panel.visible
      ? visible.rect
      : hidden.rect
  }

  Spy {
    delayed: true
    properties: [
      spy`${view}.size`,
      spy`${panel}.rect,nextRect`,
      spy`${mask}.rect`
    ]
  }
}
