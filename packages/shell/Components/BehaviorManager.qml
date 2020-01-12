import QtQuick 2.12
import org.muelle.types 1.0
import org.muelle.extra 1.0
import '../Extras'
import '../shared/functional.ts' as F
import '../libs/Flux/actions.ts' as Action

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
      store.dispatch(Action.showPanel())
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
      onEntered: store.dispatch(Action.showPanel())
      onExited: store.dispatch(Action.hidePanel())
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
        store.dispatch(Action.showPanel())
      } else {
        store.dispatch(Action.hidePanel())
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
        store.dispatch(Action.showPanel())
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
