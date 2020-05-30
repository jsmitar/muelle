import QtQuick 2.14
import org.kde.plasma.private.taskmanager 0.1 as TaskManagerApplet
import 'qrc:/shared/functional.ts' as F
import 'qrc:/shared/components'

QObject {
  id: smartLauncher

  property url launcherUrl: ''
  property QtObject launcher: null

  readonly property int count: F.get(launcher, 'count', 0)
  readonly property bool countVisible: F.get(launcher, 'countVisible', false)
  readonly property real progress: F.get(launcher, 'progress', 0)
  readonly property bool progressVisible: F.get(launcher, 'progressVisible', false)
  readonly property bool urgent: F.get(launcher, 'urgent', false)

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
