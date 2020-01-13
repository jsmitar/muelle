import QtQuick 2.12
import org.muelle.types 1.0
import '../../../Extras'

QObject {
  id: obj

  property var state

  readonly property int _panel_width:
      state.icon.size * state.panel.taskCount1 +
      state.icon.spacing * Math.max(0, state.panel.taskCount1 - 1)

  readonly property int _panel_height: Math.max(0, state.icon.size)

  readonly property int _panel_x: Math.max(0, state.icon.size)

  readonly property int _panel_y: 0

  readonly property size _panel_size: state.panel.isHorizontal
    ? Qt.size(_panel_width, _panel_height)
    : Qt.size(_panel_height, _panel_width)

  readonly property point _panel_point: state.panel.isHorizontal
    ? Qt.point(_panel_x, _panel_y)
    : Qt.point(_panel_y, _panel_x)

  property int _panel_next_width:
      state.icon.size * state.panel.nextTaskCount1 +
      state.icon.spacing * Math.max(0, state.panel.nextTaskCount1 - 1)

  readonly property int _panel_next_height: _panel_height

  property int _panel_next_x:
      state.icon.size +
      ((state.icon.size + state.icon.spacing) *
       Math.max(0, state.panel.taskCount1 - state.panel.nextTaskCount1)
      ) / 2

  readonly property int _panel_next_y: _panel_y

  readonly property size _panel_next_size: state.panel.isHorizontal
    ? Qt.size(_panel_next_width, _panel_next_height)
    : Qt.size(_panel_next_height, _panel_next_width)

  readonly property point _panel_next_point: state.panel.isHorizontal
    ? Qt.point(_panel_next_x, _panel_next_y)
    : Qt.point(_panel_next_y, _panel_next_x)

  readonly property size viewSize: state.panel.isHorizontal
    ? Qt.size(_panel_width + _panel_x * 2, _panel_height)
    : Qt.size(_panel_height, _panel_width + _panel_x * 2)

  readonly property rect viewRect:
      Qt.rect(0, 0, viewSize.width, viewSize.height)

  readonly property size panelSize: _panel_size
  readonly property point panelPoint: _panel_point
  readonly property rect panelRect:
      Qt.rect(panelPoint.x, panelPoint.y, panelSize.width, panelSize.height)

  readonly property size panelNextSize: _panel_next_size
  readonly property point panelNextPoint: _panel_next_point
  readonly property rect panelNextRect:
      Qt.rect(panelNextPoint.x, panelNextPoint.y, panelNextSize.width, panelNextSize.height)

  readonly property rect panelAbsoluteRect: Qt.rect(
    $view.x + panelRect.x, $view.y + panelRect.y,
    panelRect.width, panelRect.height
  )

  readonly property rect panelNextAbsoluteRect: Qt.rect(
    $view.x + panelNextRect.x, $view.y + panelNextRect.y,
    panelNextRect.width, panelNextRect.height
  )

  property bool maskGrow: false
  readonly property bool _hidden: state.panel.hidden && state.panel.slide !== 'Running'

  readonly property size maskSizeVisible: maskGrow
    ? viewSize
    : panelNextSize

  readonly property point maskPointVisible: maskGrow
    ? Qt.point(0, 0)
    : panelNextPoint

  readonly property size maskSizeHidden: Qt.size(1, 1)

  readonly property point maskPointHidden: Qt.point(1, 1)

  readonly property rect maskRect: _hidden
    ? Qt.rect(
        maskPointHidden.x, maskPointHidden.y,
        maskSizeHidden.width, maskSizeHidden.height
    )
    : Qt.rect(
        maskPointVisible.x, maskPointVisible.y,
        maskSizeVisible.width, maskSizeVisible.height
    )

  Spy {
    delayed: true
    properties: [
      spy`${obj}.(panelNextRect,panelRect,maskRect,_panel_next_size)`,
      spy`${state.panel}.(nextTaskCount1,taskCount1)`,
    ]
  }

}
