import QtQuick 2.14
import QtQml 2.12
import QtQuick.Controls.Universal 2.12
import org.muelle.types 1.0
import '../../shared/components'
import '../../shared/functional.ts' as F
import '../Store/actions.ts' as Action

SettingsForm {
  id: settings

  Instantiator {
    model: alignment.aligns

    BindingValue {
      target: modelData
      property: 'checked'
      initial: $layout.alignment === modelData.value
      then: () => {
        store.dispatch(Action.changeAlignment(target.value))
      }
    }
  }

  BindingValue {
    target: alignment.centerOffset
    property: 'value'
    initial: $positioner.centerOffset

    readonly property var updatingOffset: F.debounce((v) => {
      store.state.panel.updatingOffset = v
    }, 500)

    then: offset => {
      if (!store.state.panel.updatingOffset) {
        store.state.panel.updatingOffset = true
      }
      store.state.geometry.panelOffset = offset
      updatingOffset(false)
    }
    otherwise: then
  }

  Instantiator {
    model: edge.edges
    BindingValue {
      target: modelData
      property: 'checked'
      initial: $layout.edge === modelData.value
      then: () => store.dispatch(Action.changeEdge(target.value))
    }
  }

  Instantiator {
    model: behavior.behaviors
    BindingValue {
      target: modelData
      property: 'checked'
      initial: store.state.panel.behavior === modelData.value
      then: F.debounce(() => {
          store.dispatch(Action.changeBehavior(target.value))
        }, 1000)
    }
  }
}
