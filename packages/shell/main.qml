import QtQuick 2.14
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtGraphicalEffects 1.12
import org.muelle.types 1.0
import org.muelle.extra 1.0
import org.kde.taskmanager 0.1
import org.kde.plasma.core 2.0

import 'Components'
import 'Containers'
import 'Settings'
import 'Store/actions.ts' as Action
import '../shared/functional.ts' as F
import '../shared/components'
import 'Store'

Item {
  id: root
  objectName: 'root'

  width: store.state.geometry.viewSize.width
  height: store.state.geometry.viewSize.height

  Component.onCompleted: {
    // START: Set Global Properties
    Qt.Muelle = { Types }
    Qt.setTimeout = setTimeout
    Qt.setInterval = setInterval
    Qt.clearTimeout = clearTimeout
    Qt.clearInterval = clearInterval
    Qt.__muelle_separator__ = __muelle_separator__
    // END: Set Global Properties
    console.log(F.tostr($container.screens, 3))
  }

  PaintItem {
    id: bg
    target: root
    enabled: false
    showSize: true
  }

  readonly property SettingsWindow settings: SettingsWindow {}

  readonly property ShellStore store: ShellStore {
    id: store

    Binding {
      target: $layout
      property: 'edge'
      value: store.state.panel.edge
    }
    Binding {
      target: $layout
      property: 'alignment'
      value: store.state.panel.alignment
    }
    Binding {
      target: $view
      property: 'size'
      value: store.state.geometry.viewSize
      delayed: true
    }
    Binding {
      target: $view
      property: 'mask'
      value: store.state.geometry.maskRect
      delayed: true
    }
    Binding {
      target: $view
      property: 'panelPosition'
      value: store.state.geometry.panelNextPoint
      delayed: true
    }
    Binding {
      target: $view
      property: 'panelSize'
      value: store.state.geometry.panelNextSize
      delayed: true
    }
    Connections {
      enabled: store.state.tasksModel.ready
      target: store.state.tasksModel

      onLauncherListChanged: {
        $configuration.launcherList = JSON.stringify(target.launcherList)
        $configuration.save()
      }
    }
    Connections {
      target: store.state.tasksModel

      onCountChanged: {
        store.dispatch(Action.updateTaskCount1({ 
          tasks: target.count, 
          launcherList: target.launcherList
        }))
      }
    }

    contextMenu: DockMenu {
      parent: root
      model: [{
        text: 'Preferences',
        trigger() { settings.visible = true }
      }].concat(Object.values($container.screens)
        .filter(screen => screen.name !== $view.screen.name)
        .map(screen => ({
          text: `${screen.name} (${screen.model})`,
          trigger() { 
            $view.screen = screen 
            $positioner.update(0)
          }
        }))
      )
    }
  }

  // ViewShadows {
  //   view: $view
  //   geometry: store.state.geometry.backgroundRect
  //   enabled: store.state.panel.visible && false
  //   edges: 
  //     (Types.Top | Types.Right | Types.Bottom | Types.Left) 
  //     & ~(store.state.panel.edge)
  // }

  PanelBackground {
    id: panelBackground
  }

  DockContainer {
    id: dockArea
    anchors.fill: root
  }

  DockMenu {}

  BehaviorManager {}
}
