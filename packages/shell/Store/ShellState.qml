import QtQuick 2.12
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
    padding: 4
    spacing: 4
  }

  readonly property Background background: Background {
    parentObject: state
    lift: 10
    shadowBlur: 5
    borderRadius: 30
    paddingX: 10
    paddingY: 2
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
