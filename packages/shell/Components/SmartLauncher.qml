import QtQuick 2.14
import org.kde.plasma.private.taskmanager 0.1 as TaskManagerApplet

import '../../shared/components'

QObject {
  id: smartLauncher

  property url launcherUrl: ''
  property QtObject launcher: null

  readonly property int count: launcher.count || 0
  readonly property bool countVisible: launcher.countVisible || false
  readonly property real progress: launcher.progress || 0
  readonly property bool progressVisible: launcher.progressVisible || false
  readonly property bool urgent: launcher.urgent || false

  onObjectNameChanged: {
    if (objectName) {
      launcher = memoComponent.createObject(
        smartLauncherComponent, `${objectName}:SmartLauncher`
      )
      launcher.launcherUrl = Qt.binding(() => launcherUrl)
    }
  }

  Component {
    id: smartLauncherComponent
    TaskManagerApplet.SmartLauncherItem { }
  }
}
