import QtQuick 2.12
import './flux.ts' as FluxStore

QtObject {
  property var store
  property var state
  property var mutations
  property var sagaRoot

  Component.onCompleted: {
    store = new FluxStore({
      state,
      mutations,
      sagaRoot
    })
  }

  function dispatch(action) {
    store.dispatch(action)
  }
}
