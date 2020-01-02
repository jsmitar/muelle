import QtQuick 2.10
import '../../Extras'
import '../functional.ts' as F

QObject {
  id: store
  objectName: 'FluxStore'

  property FluxState state
  property var actions
  property var mutations

  property bool debug: false

  function commit(type, payload) {
    if (!(type in mutations.mutations)) {
      throw new Error(`FluxStore.commit: ${type} did not recognized`)
    }

    if (debug) {
       console.log(`[COMMIT] - ${type}`, payload !== undefined ? F.tostr(payload) : '')
    }
    mutations.mutations[type](payload)
  }

  function dispatch({ type, payload }) {
    if(!(type in actions)) {
      throw new Error(`FluxStore.dispatch: ${type} did not recognized`)
    }

    if (debug) {
       console.log(`[ACTION] - ${type}`, payload !== undefined ? F.tostr(payload) : '')
    }
    return Qt.Promise
      .resolve(actions[type](payload))
  }

}
