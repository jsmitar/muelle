import QtQuick 2.4
import QtQml 2.12
import QtQuick.Controls.Universal 2.3
import org.muelle.types 1.0
import "../Extras"

SettingsForm {
  id: settings

  onVisibleChanged: {
    if (visible) {
      store.dispatch({ type: 'showPanel' })
    }
  }

  Instantiator {
    model: alignment.aligns
    BindingValue {
      target: modelData
      initial: $layout.alignment === modelData.value
      when: 'checked'
      then: ({ value }) => {
        store.dispatch({ type: 'changeAlignment', payload: value })
      }
    }
  }

  BindingValue {
    target: alignment.centerOffset
    initial: $positioner.centerOffset
    when: 'value'
    then: ({ value }) => {
      $positioner.centerOffset = value
      Qt.callLater(() => $positioner.update(100))
    }
    otherwise: then
  }

  Instantiator {
    model: edge.edges
    BindingValue {
      target: modelData
      initial: $layout.edge === modelData.value
      when: 'checked'
      then: ({ value }) => {
        store.dispatch({ type: 'changeEdge', payload: value })
      }
    }
  }

  Instantiator {
    model: behavior.behaviors
    BindingValue {
      target: modelData
      initial: $behavior.behavior === modelData.value
      when: 'checked'
      then: target => $behavior.behavior = target.value
    }
  }
}
