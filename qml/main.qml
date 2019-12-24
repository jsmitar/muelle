import QtQuick 2.12
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtQuick.Window 2.3
import org.muelle.types 1.0
import org.muelle.extra 1.0
import org.kde.taskmanager 0.1
import "Components"
import "Containers"
import "Extras"
import "Settings"
import "libs/Flux"
import "libs/Flux/actions.js" as Action
import "libs/functional.js" as F
import "libs/signal.js" as Signal
import "polyfills/Timer/timer.js" as TimerPolyfill
import "polyfills/promise.js" as PromisePolyfill

import "libs/saga.js" as Saga

Item {
  id: root
  objectName: 'root'

  width: store.state.geometry.viewSize.width
  height: store.state.geometry.viewSize.height

  Component.onCompleted: {
    $positioner.centerOffset = 0
    Qt.Promise.all([
      store.dispatch(Action.changeEdge(Types.Bottom)),
      store.dispatch(Action.changeAlignment(Types.Center)),
      store.dispatch(Action.changeBehavior(Types.DodgeActive))
    ])
    Saga.run(Saga.test)
  }

  PaintItem {
    target: root
    enabled: false
    showSize: false
  }

  readonly property SettingsWindow settings: SettingsWindow {}

  readonly property AnimationSlide slide: AnimationSlide {}

  readonly property Store store: Store {
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
      property: 'panelGeometry'
      value: store.state.geometry.panelNextRect
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
