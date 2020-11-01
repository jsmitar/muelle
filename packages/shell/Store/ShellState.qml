import QtQuick 2.14
import org.muelle.types 1.0
import org.kde.taskmanager 0.1
import 'qrc:/shared/components'
import 'state'

QObject {
  id: state
  objectName: '@State'

  readonly property Settings settings: Settings { parentObject: state }

  readonly property Panel panel: Panel { parentObject: state }

  readonly property Icon icon: Icon {
    parentObject: state
    iconSize: 70
    padding: 8
    spacing: 4
  }

  readonly property Background background: Background {
    parentObject: state
    shadowBlur: 15
    paddingX: 0.2
    paddingTop: 0
    paddingBottom: 0.02
    inset: 1
    style: 'solid'
    radius.topLeft: 3
    radius.topRight: 3
    radius.bottomLeft: 3
    radius.bottomRight: 3
    fill: false
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
      launcherList = JSON.parse($configuration.launcherList.replace('__muelle_separator__', __muelle_separator__) || '[]')
      Qt.callLater(countChanged)
      ready = true
    }
  }
}
