import QtQuick 2.14
import 'qrc:/shared/components'

QtObject {
  property Item target
  property bool running: false

  onRunningChanged: {
    if (running && objectName) {
      const animation = memoComponent.createObject(
        c.startup,
        `${objectName}:StartupAnimation`
      )
      animation.start()
    }
  }

  onObjectNameChanged: {
    if (objectName) {
      const animation = memoComponent.createObject(
        c.startup,
        `${objectName}:StartupAnimation`
      )

      target.rotation = Qt.binding(() => animation.rotation)
    }
  }
}