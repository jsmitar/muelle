import QtQuick 2.12
import 'flux.ts' as Flux
import '../components'
import '../saga/simpleSagaMonitor.ts' as S

QObject {
  property QtObject _state
  property var mutations
  property var sagaRoot
  property var dispatch

  on_StateChanged: private_.init()
  onMutationsChanged: private_.init()
  onSagaRootChanged: private_.init()

  QObject {
    id: private_
    property var store

    function init() {
      if (state && mutations && sagaRoot && !store) {
        store = new Flux.FluxStore({
          state,
          mutations,
          sagaRoot,
          sagaMonitor: S.simpleSagaMonitor
        })
        parentObject.dispatch = payload => store.dispatch(payload)
      }
    }
  }
}
