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
    padding: 5
    spacing: 8
  }

  readonly property Geometry geometry: Geometry {
    parentObject: state
    state: state
  }

  readonly property Animation animation: Animation {
    parentObject: state
  }

  readonly property Context context: Context {}

  readonly property TasksModel tasksModel: TasksModel {
    virtualDesktop: store.virtualDesktopInfo.currentDesktop
    activity: store.activityInfo.currentActivity
    launchInPlace: true
    separateLaunchers: true
    groupMode: TasksModel.GroupApplications
    groupInline: true
    groupingWindowTasksThreshold: -1
    filterByVirtualDesktop: false
    filterByScreen: false
    filterByActivity: true
    sortMode: TasksModel.SortManual
    launcherList: `applications:org.qt-project.qtcreator.desktop,applications:firefox.desktop,applications:code.desktop,applications:telegramdesktop.desktop,applications:org.kde.dolphin.desktop`.split(',')

    // onCountChanged: {
    //   store.dispatch(Action.updateTaskCount1(count))
    // }

    Component.onCompleted: {
      countChanged()
    }
  }

}
