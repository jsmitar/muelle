import QtQuick 2.14

QtObject{
  objectName: `Task:Group`

  property var m
  property bool hover: false
  property Item visualParent

  Component.onDestruction: {
    hover = false
    visualParent = null
  }

  onHoverChanged: {
    if (hover) {
      const taskgroup = memoComponent.createObject(c_TaskGroup, objectName)
      taskgroup.m = Qt.binding(() => m)
      taskgroup.visualParent = Qt.binding(() => visualParent)
      taskgroup.hover = Qt.binding(() => hover)
    }
  }
}