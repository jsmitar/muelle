.pragma library

const __instances = []

class SignalStore {
  constructor() {
    __instances.push(this)
    this.connections = {} || new Proxy({}, {
      set(obj, prop, value) {
        if (prop in obj) throw new Error('SignalStore: key already set')
        Reflect.set(obj, prop, value);
      },
    })

    this.disconnectHooks = []

    this.invalid = false

    this.id = (function* () {
      let id = 0
      while(true) yield ++id
    })()
  }

  connect(signal, slot) {
    signal.disconnect(slot)
    signal.connect(slot)

    const id = this.id.next().value

    const clear = () => {
      delete this.connections[id]
      signal.disconnect(slot)
    }

    this.connections[id] = clear

    return clear
  }

  connectOnce(signal, slot) {
    const clear = this.connect(signal, slot)
    signal.connect(clear)

    return clear
  }

  disconnectWhen(signal, predicate) {
    const clear = function () {
      if (predicate === undefined || predicate()) {
        for (let k in this.connections) {
          this.connections[k]()
        }

        this.disconnectHooks
            .forEach(([signal, slot]) => signal.disconnect(slot))
        this.disconnectHooks = []
      }
    }
    signal.connect(signal, clear.bind(this))
    this.disconnectHooks.push([signal, clear])
  }

  release() {
    for (let k in this.connections) {
      this.connections[k]()
    }

    this.disconnectHooks
        .forEach(([signal, slot]) => signal.disconnect(slot))
  }
}


class Signal {
  constructor() {
    this.store = new SignalStore()
  }

  on(signals, slot) {
    signals = Array.isArray(signals) ? signals : [signals]

    const connections = signals.map(signal => {
      return this.store.connect(signal,
        slot.bind({
          off() { connections.forEach(off => off()) }
        })
      )
    })

    return this
  }

  once(signals, slot) {
    signals = Array.isArray(signals) ? signals : [signals]

    const connections = signals.map(signal => {
      return this.store.connect(signal, () => {
        connections.forEach(off => off())
        slot()
      })
    })

    return this
  }

  offWhen(signal, predicate) {
    this.store.disconnectWhen(signal, predicate)

    return this
  }
}

function create() {
  return new Signal()
}

function release() {
  __instances.forEach(store => store.release())
}
