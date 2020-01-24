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
    sortMode: TasksModel.SortManual
    launcherList: []

    onCountChanged: {
      store.dispatch(Action.updateTaskCount1(count))
      launchInPlace = true
      launcherList = `applications:org.qt-project.qtcreator.desktop,applications:firefox.desktop,applications:code.desktop,applications:telegramdesktop.desktop,applications:org.kde.dolphin.desktop,file:///home/smith/Descargas/MellowPlayer-x86_64.AppImage`.split(',')
    }

    Component.onCompleted: {
      tasksModel.countChanged()
//      groupingAppIdBlacklist = plasmoid.configuration.groupingAppIdBlacklist;
//      groupingLauncherUrlBlacklist = plasmoid.configuration.groupingLauncherUrlBlacklist;
    }

//    readonly property int logicalLauncherCount: {
//    if (plasmoid.configuration.separateLaunchers) {
//    return launcherCount;
//    }

//    var startupsWithLaunchers = 0;

//    for (var i = 0; i < taskRepeater.count; ++i) {
//    var item = taskRepeater.itemAt(i);

//    if (item && item.m.IsStartup === true && item.m.HasLauncher === true) {
//    ++startupsWithLaunchers;
//    }
//    }

//    return launcherCount + startupsWithLaunchers;
//    }

//    onUrlsDropped: {
//    // If all dropped URLs point to application desktop files, we'll add a launcher for each of them.
//    var createLaunchers = urls.every(function (item) {
//    return backend.isApplication(item)
//    });

//    if (createLaunchers) {
//    urls.forEach(function (item) {
//    addLauncher(item);
//    });
//    return;
//    }

//    if (!hoveredItem) {
//    return;
//    }

//    // DeclarativeMimeData urls is a QJsonArray but requestOpenUrls expects a proper QList<QUrl>.
//    var urlsList = backend.jsonArrayToUrlList(urls);

//    // Otherwise we'll just start a new instance of the application with the URLs as argument,
//    // as you probably don't expect some of your files to open in the app and others to spawn launchers.
//    tasksModel.requestOpenUrls(hoveredItem.modelIndex(), urlsList);
//    }


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
