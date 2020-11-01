import QtQuick 2.14
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import QtGraphicalEffects 1.12
import org.muelle.types 1.0
import org.muelle.extra 1.0
import org.kde.taskmanager 0.1
import org.kde.plasma.core 2.0

import 'qrc:/shared/components'
import 'qrc:/shared/functional.ts' as F

import 'qrc:/shell/Settings'
import 'qrc:/shell/Containers'
import 'qrc:/shell/Components'
import 'qrc:/shell/Store'
import 'qrc:/shell/Store/actions.ts' as Action
import 'qrc:/shell/helpers/memoComponent.ts' as MC
import 'qrc:/shell/helpers'

Item {
  id: root
  objectName: 'root'

  width: store.state.geometry.view.size.width
  height: store.state.geometry.view.size.height

  readonly property var memoComponent: MC.createMemoComponent($view.release, root)
  readonly property Components c: Components {}

  readonly property TaskGroupComponent c_TaskGroup: TaskGroupComponent {}

  Component.onCompleted: {
    console.info('Shell created')
  }

  Component.onDestruction: {
    console.info('Shell destroyed')
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
      value: store.state.geometry.view.size
      delayed: true
    }
    Binding {
      target: $view
      property: 'inputMask'
      value: store.state.geometry.mask.region
      delayed: true
    }
    Binding {
      target: $view
      property: 'panelGeometry'
      value: store.state.geometry.mask.visible.region
      delayed: true
    }
    Binding {
      target: $view
      property: 'frameExtents'
      value: store.state.geometry.background.extents
    }
    Connections {
      enabled: store.state.tasksModel.ready
      target: store.state.tasksModel

      function onLauncherListChanged() {
        $configuration.launcherList = JSON.stringify(target.launcherList)
              .replace(__muelle_separator__, '__muelle_separator__')
        $configuration.save()
      }
    }
    Connections {
      target: $view.screen

      function onGeometryChanged() {
        setTimeout(() => {
          $positioner.update(0)
        }, 1000)
      }
    }
    Connections {
      target: store.state.tasksModel

      function onCountChanged() {
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
            store.dispatch(Action.changeScreen(screen.name))
          }
        }))
      )
    }
  }

  // ViewShadows {
  //   view: $view
  //   geometry: store.state.geometry.background.rect
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

  // GeometryDebug {
  //   visible: true
  // }

  // Item {
  //   id: mask
  //   property rect rect: store.state.geometry.mask.
  //   x: rect.x
  //   y: rect.y
  //   width: rect.width
  //   height: rect.height

  //   PaintItem {
  //     target: mask; enabled: false
  //   }
  //   z: -1
  // }
}
