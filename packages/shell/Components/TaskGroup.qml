import QtQuick 2.14
import 'qrc:/shared/functional.ts' as F

QtObject{
  objectName: `Task:Group`

  property var m
  property bool hover: false
  property Item visualParent

  readonly property QtObject taskgroup: 
    memoComponent.createObject(c_TaskGroup, objectName)

  Component.onCompleted: {
    F.on(hoverChanged, () => {
      taskgroup.hover = false
    })
    F.on(hoverChanged, F.debounce(show, 250))
  }

  Component.onDestruction: {
    hover = false
    visualParent = null
  }

  function show() {
    if (hover) {
      taskgroup.m = Qt.binding(() => m)
      taskgroup.visualParent = Qt.binding(() => visualParent)
      taskgroup.hover = Qt.binding(() => hover)
    } else {
      taskgroup.hover = false
    }
  }
}