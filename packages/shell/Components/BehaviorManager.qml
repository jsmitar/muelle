import QtQuick 2.12
import org.muelle.types 1.0
import org.muelle.extra 1.0
import '../../shared/components'
import '../../shared/functional.ts' as F
import '../Store/actions.ts' as Action

QObject {
  id: manager
  objectName: 'BehaviorManager'

  signal forceUpdate

  PressureDetector {
    id: pressure
    view: $view
    edge: store.state.panel.edge
    threshold: 20
    enabled: store.state.panel.hidden

    onThresholdReached: {
      store.dispatch(Action.slideIn())
    }
  }

  PanelBehavior {
    id: behavior
    view: $view
    behavior: store.state.panel.behavior
  }

  ConnectionGroup {
    id: autoHide
    enabled: behavior.behavior === Types.AutoHide

    Connections {
      target: $view
      onEntered: store.dispatch(Action.slideIn())
      onExited: store.dispatch(Action.slideOut())
    }
  }

  ConnectionGroup {
    id: dodgeWindow
    enabled: [Types.DodgeAll, Types.DodgeActive].includes(behavior.behavior)

    onEnabledChanged: {
      enabled && forceUpdate()
    }

    function dodge() {
      if ($view.containsMouse || !behavior.dodge) {
        if (store.state.panel.slide === 'slide-out-running' ||
            store.state.panel.hidden
        ) {
          store.dispatch(Action.slideIn())
        }
      } else {
        if (store.state.panel.slide === 'slide-in-running' ||
            store.state.panel.visible
        ) {
          store.dispatch(Action.slideOut())
        }
      }

    }

    Connections {
      target: $view
      onEntered: dodgeWindow.dodge()
      onExited: dodgeWindow.dodge()
    }
    Connections {
      target: behavior
      onDodgeChanged: dodgeWindow.dodge()
    }
    Connections {
      target: manager
      onForceUpdate: dodgeWindow.dodge()
    }
  }

  ConnectionGroup {
    id: alwaysVisible
    enabled: behavior.behavior === Types.AlwaysVisible

    property var updateStruts:
      F.debounce(() => enabled && behavior.updateStruts(), 1000)

    onEnabledChanged: {
      if (enabled) {
        forceUpdate()
        store.dispatch(Action.slideIn())
      }
    }

    Connections {
      target: store.state.geometry
      onPanelSizeChanged: alwaysVisible.updateStruts()
    }
    Connections {
      target: manager
      onForceUpdate: alwaysVisible.updateStruts()
    }
  }
}
