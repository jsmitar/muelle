import QtQuick 2.12
import QtQuick.Layouts 1.12
import QtQuick.Controls 2.12
import QtQuick.Window 2.12
import org.muelle.types 1.0
import org.muelle.extra 1.0
import org.kde.taskmanager 0.1

import 'Components'
import 'Containers'
import 'Settings'
import 'Store/actions.ts' as Action
import '../shared/functional.ts' as F
import '../shared/components'
import '../shared/saga/test.ts' as Saga
import 'Store'

Item {
  id: root
  objectName: 'root'

  width: store.state.geometry.viewSize.width
  height: store.state.geometry.viewSize.height

  Component.onCompleted: {
    // Start: Set Globals
    Qt.setTimeout = setTimeout
    Qt.clearTimeout = clearTimeout
    Qt.Muelle = { Types }
    $positioner.centerOffset = 0
    // End: Set Globals
  }

  PaintItem {
    target: root
    enabled: true
    showSize: false
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

    contextMenu: DockMenu {
      parent: root
      model: [{
        text: 'Preferences',
        trigger: () => { settings.visible = true }
      }]
    }
  }

  DockContainer {
    id: dockArea
    anchors.fill: root
  }

  DockMenu {}

  BehaviorManager {}
}
