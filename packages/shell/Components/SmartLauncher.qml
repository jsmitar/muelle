import QtQuick 2.14
import 'qrc:/shared/functional.ts' as F

QtObject {
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
        c.smartLauncher, `${objectName}:SmartLauncher`
      )
      launcher.launcherUrl = Qt.binding(() => launcherUrl)
    }
  }
}
