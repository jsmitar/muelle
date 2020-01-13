import QtQuick 2.12
import './flux.ts' as Flux
import '../components'

QObject {
  property var state
  property var mutations
  property var sagaRoot
  property var dispatch

  onStateChanged: private_.init()
  onMutationsChanged: private_.init()
  onSagaRootChanged: private_.init()

  QObject {
    id: private_
    property var store

    function init() {
      if (state && mutations && sagaRoot) {
        store = new Flux.FluxStore({
          state,
          mutations,
          sagaRoot
        })
        parentObject.dispatch = store.dispatch.bind(store)
      }
    }
  }
}
