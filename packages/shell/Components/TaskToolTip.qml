import QtQuick 2.14

QtObject{
  objectName: `Task:ToolTip`

  property string text: ''
  property bool visible: false
  property Item visualParent

  Component.onDestruction: {
    visible = false
    visualParent = null
  }

  onVisibleChanged: {
    if (visible) {
      const tooltip = memoComponent.createObject(c.tooltip, objectName)
      tooltip.text = Qt.binding(() => text)
      tooltip.visualParent = Qt.binding(() => visualParent)
      tooltip.visible = Qt.binding(() => visible)
    }
  }
}