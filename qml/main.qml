import QtQuick 2.11
import QtQuick.Layouts 1.1
import QtQuick.Controls 2.2
import QtQuick.Window 2.3
import org.duckdock.types 1.0
import org.duckdock.extra 1.0
import org.kde.taskmanager 0.1
import "Components"
import "Containers"
import "Extras"
import "Settings"
import "libs/Flux"
import "libs/functional.js" as F
import "polyfills/Timer/timer.js" as TimerPolyfill
import "polyfills/promise.js" as PromisePolyfill

Item {
  id: root

  width: store.state.geometry.viewSize.width
  height: store.state.geometry.viewSize.height

  PaintItem {
    target: root
    enabled: false
    showSize: false
  }

  readonly property AnimationSlide slide: AnimationSlide {}

  readonly property Store store: Store {
    debug: true
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
    }
    Binding {
      target: $view
      property: 'mask'
      value: store.state.geometry.maskRect
    }
    Binding {
      target: $view
      property: 'panelGeometry'
      value: store.state.geometry.panelNextRect
    }

    Connections {
      target: store.state.geometry
    }

    contextMenu: DockMenu {
      parent: root
      model: [{
        text: 'Preferences',
        trigger: () => { settings.visible = true }
      }]
    }
  }

  Component.onCompleted: {
    $positioner.centerOffset = 0
    $view.setOpacity(0)

    Qt.Promise.all([
      store.dispatch({ type: "changeEdge", payload: Types.Top }),
      store.dispatch({ type: "changeAlignment", payload: Types.Center })
    ])
    .then(() => {
      store.dispatch({ type: "showPanel" })
    })
  }

  Connections {
    target: $behavior
    enabled: !settings.visible
    onDodgeChanged: {
      if ($behavior.dodge) {
        store.dispatch({ type: 'hidePanel' })
      }
    }
  }

  DockContainer {
    id: dockArea
    anchors.fill: root
  }

  DockMenu {}

  PressureDetector {
    id: pressureDetector

    edge: $layout.edge
    view: $view
    threshold: 2
    enabled: store.state.panel.hidden

    onThresholdReached: {
      store.dispatch({ type: 'showPanel' })
    }
  }

  Connections {
    target: $view

    property Timer hideTimer: Timer {
      id: hideTimer
      repeat: false
    }

    onExited: {
      const s = settings
      if (s.visible) return

      const clear = F.timeout(hideTimer, () => {
        store.dispatch({ type: 'hidePanel' })
      }, 1000)
      F.once($view.entered, () => {
        clear();
        if (!s.visible) {
          root.store.dispatch({ type: 'showPanel' })
        }
      })
      pressureDetector.updateBarrier()
    }

  }

  Rectangle {
    id: bg
    width: store.state.geometry.panelNextRect.width
    height: store.state.geometry.panelNextRect.height

    Behavior on width {
      NumberAnimation {
        duration: store.state.animation.duration / 2
        easing.type: Easing.InOutQuad
      }
    }

    anchors.centerIn: parent
    antialiasing: true
    z: -1
    color: "#34373A"
    radius: 4
    transform: Rotation {
      angle: -60
      origin.x: bg.width / 2
      origin.y: 0
      axis.x: 1
      axis.y: 0
      axis.z: 0
    }
  }

  SettingsWindow {
    id: settings
    flags: Qt.WindowStaysOnTopHint
  }
}
