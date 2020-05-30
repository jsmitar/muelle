import QtQuick 2.14
import org.muelle.types 1.0
import org.kde.taskmanager 0.1
import '../../shared/components'
import 'state'

QObject {
  id: state
  objectName: '@State'

  readonly property Settings settings: Settings { parentObject: state }

  readonly property Panel panel: Panel { parentObject: state }

  readonly property Icon icon: Icon {
    parentObject: state
    iconSize: 48
    padding: 2
    spacing: 4
  }

  readonly property Background background: Background {
    parentObject: state
    shadowBlur: 10
    paddingX: 1
    paddingTop: -0.5
    paddingBottom: 0
    inset: 0
    style: 'solid'
    radius.topLeft: 0
    radius.topRight: 0
    radius.bottomLeft: 0
    radius.bottomRight: 0
  }
  
  readonly property Geometry geometry: Geometry {
    parentObject: state
    state: state
  }

  readonly property Animation animation: Animation {
    parentObject: state
    duration: 500
  }

  readonly property Context context: Context {}

  readonly property TasksModel tasksModel: TasksModel {
    virtualDesktop: store.virtualDesktopInfo.currentDesktop
    activity: store.activityInfo.currentActivity
    launchInPlace: true
    separateLaunchers: true
    groupMode: TasksModel.GroupApplications
    groupInline: false
    groupingWindowTasksThreshold: -1
    filterByVirtualDesktop: false
    filterByScreen: false
    filterByActivity: true
    sortMode: TasksModel.SortManual

    property bool ready: false

    Component.onCompleted: {
      launcherList = JSON.parse($configuration.launcherList || '[]')
      Qt.callLater(countChanged)
      ready = true
    }
  }
}
