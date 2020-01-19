import QtQuick 2.12
import org.kde.taskmanager 0.1
import org.kde.plasma.private.taskmanager 0.1 as TaskManagerApplet

import '../Components'
import '../../shared/components'
import '../../shared/functional.ts' as F
import '../../shared/flux'
import 'mutations.ts' as M
import 'sagaRoot.ts' as Saga
import 'actions.ts' as Action

FluxStore {
  id: store

  readonly property ShellState state: ShellState {}
  _state: state
  mutations: M.mutations
  sagaRoot: Saga.sagaRoot

  property DockMenu contextMenu

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

    onCountChanged: {
      store.dispatch(Action.updateTaskCount1(count))
    }

    Component.onCompleted: {
      tasksModel.countChanged()
    }
  }

  readonly property ActivityInfo activityInfo:
    ActivityInfo {}

  readonly property VirtualDesktopInfo virtualDesktopInfo:
    VirtualDesktopInfo {}

  readonly property TaskManagerApplet.Backend backend: TaskManagerApplet.Backend {
    taskManagerItem: root
    toolTipItem: Rectangle {
      color: "#c83232"
      visible: true
    }
    onAddLauncher: {
      tasksModel.requestAddLauncher(url)
    }
  }
}
