.pragma library

class SignalStore {
  constructor() {
    this.nextId = 0
    this.storeConnection = {}
  }

  functionId(signal) {
    if (signal.__id__) {
      return signal.__id__
    }
    signal.__id__ = ++this.nextId
    return signal.__id__
  }

  getStore(signal) {
    const id = this.functionId(signal)

    if (!this.storeConnection[id]) {
      return this.storeConnection[id] = {}
    }

    return this.storeConnection[id]
  }

  isConnected(signal, slot) {
    const store = this.getStore(signal)
    const slotId = this.functionId(slot)

    return !!store[slotId]
  }

  /**
    * @brief: connect a signal to a slot and save the connection into store
    * @returns: disconnect()
    */
  connect(signal, slot, once = false) {
    const store = this.getStore(signal)
    const slotId = this.functionId(slot)

    store[slotId] = slot

    const clear = () => this.disconnect(signal, slot)
    signal.connect(slot)

    if (once && !this.isConnected(signal, slot)) {
      signal.connect(clear)
    }

    return clear
  }

  /**
    * @brief: connect a signal to a slot and clean the connection into store
    * @returns: disconnect()
    */
  disconnect(signal, slot) {
    const store = this.getStore(signal)
    const slotId = this.functionId(slot)
    delete store[slotId]

    if (!Object.keys(store).length) {
      delete this.storeConnection[this.functionId(signal)]
    }

    signal.disconnect(slot)
  }

  /**
    * @brief disconnect all connections saved into the store
    */
  disconnectAll(signal) {
    const store = this.getStore(signal)

    for (var slotId in store) {
      disconnect(signal, this.store[slotId])
    }
  }
}

const Signal = new SignalStore()

/**
  * @returns {disconnect: Function}
  */
function on(signal, slot) {
  try {
    return Signal.connect(signal, slot)
  } catch (e) {
    console.warn(e)
  }
}

/**
  * @brief: connect a set signals to a slot, and when the slot is triggered
  *   all signals are disconnected
  * @returns: disconnect function
  */
function race(signals = [], slot) {
  try {
    const disconnect = () =>
      signals.forEach(signal => Signal.disconnect(signal, slot))

    const _slot = () => {
      disconnect(); slot()
    }

    signals.forEach(signal => {
      Signal.connect(signal, _slot)
    })

    return disconnect
  } catch(e) {
    console.warn(e)
  }
}

/**
  * @brief: connect an signal to a slot just oncetime
  * @returns: disconnect function
  */
function once(signal, slot) {
  try {
    return Signal.connect(signal, slot, true)
  } catch(e) {
    console.warn(e)
  }
}

/**
  * args: args (n-1) are signals
  * slot: last argument is considered a slot
  */
function disconnect(...args) {
  if (args.length < 2) throw new Error('insufficient args')

  try {
    const signals = args.slice(0, -1)
    const slot = args[args.length - 1]

    signals.forEach(signal => Signal.disconnect(signal, slot))
  } catch(e) {
    console.warn(e)
  }
}


function disconnectAll(...signals) {
  try {
    signals.forEach(signal => Signal.disconnectAll(signal))
  } catch(e) {
    console.warn(e)
  }
}
