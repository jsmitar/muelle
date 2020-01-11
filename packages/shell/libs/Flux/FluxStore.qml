import QtQuick 2.12
import '../../Extras'
import '../../shared/functional.ts' as F

QObject {
  id: store
  objectName: 'FluxStore'

  property FluxState state
  property var actions
  property var mutations

  property bool debug: false

  function commit(type, payload) {
    if (!type || !(type in mutations.mutations)) {
      throw new Error(`FluxStore.commit: ${type} did not recognized`)
    }

    if (debug) {
       console.log(`[COMMIT] - ${type}`, payload !== undefined ? F.tostr(payload) : '')
    }
    try {
      mutations.mutations[type](payload)
    } catch (e) {
      throw new Error(`FluxStore.commit: fail to call mutation: ${type}`)
    }
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
