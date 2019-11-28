import QtQuick 2.10
import '../../Extras'
import '../functional.js' as F

QObject {
  id: store
  objectName: 'FluxStore'

  property FluxState state
  property var actions
  property var mutations

  property bool debug: false

  function commit(type, payload) {
    console.assert(type in mutations.mutations, `FluxStore.commit: ${type} did not recognized`)
    if (debug) {
       console.log(`[COMMIT] - ${type}`, payload !== undefined ? F.tostr(payload) : '')
    }
    mutations.mutations[type](payload)
  }

  function dispatch({ type, payload }) {
    console.assert(type in actions, `FluxStore.dispatch: ${type} did not recognized`)
    if (debug) {
       console.log(`[ACTION] - ${type}`, payload !== undefined ? F.tostr(payload) : '')
    }
    return Qt.Promise
      .resolve(actions[type](payload))
  }

}
