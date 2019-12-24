function call(fn, ...args) {
  return { type: 'call', fn, args }
}

function fork(saga, ...args) {
  return { type: 'fork', saga, args }
}

function race(effects) {
  return { type: 'race', effects }
}

function all(effects) {
  return { type: 'all', effects }
}

function take(pattern) {
  return { type: 'take', action: pattern }
}

function cancelled() {
  return { type: 'cancelled' }
}

function cancel() {
  return { type: 'cancel' }
}

function join(task) {
  return { type: 'join', task }
}

function delay(time) {
  return call(next => {
    Qt.setTimeout(next, time)
  })
}

function takeEvery(pattern, saga) {
  return fork(function *() {
    while(true) {
      const action = yield take(pattern)
      yield fork(saga, action)
    }
  })
}

function put(pattern, payload) {
  return { type: 'put', action: { type: pattern, payload }}
}

class EventEmitter {
  constructor() {
    this.listeners = { '*': {} }
    this.nextId = 0
  }

  emit(event, payload) {
    if (event in this.listeners) {
      Object.values(this.listeners[event])
        .forEach(listener => listener(payload, event))
    }

    Object.values(this.listeners['*'])
      .forEach(listener => listener(payload, event))
  }

  all(events, listener) {
    const payloads = new Map()

    const cb = (payload, event) => {
      payloads.set(event, payload)

      if (payloads.size === events.length) {
        events.forEach(event => this.off(event, cb))
        listener(payloads.values())
      }
    }
    cb._id_ = ++this.nextId

    events.forEach(event => {
      if (!(event in this.listeners))
        this.listeners[event] = {}

      this.listeners[event][cb._id_] = cb
    })

    return () => events.forEach(event => this.off(event, cb))
  }

  race(events, listener) {
    const cb = (payload, event) => {
      events.forEach(event => this.off(event, cb))

      const payloads = Array.from({ length: events.length })
      payloads.splice(events.indexOf(event), 1, payload)

      listener(payloads, event)
    }
    cb._id_ = ++this.nextId

    events.forEach(event => {
      if (!(event in this.listeners))
        this.listeners[event] = {}

      this.listeners[event][cb._id_] = cb
    })

    return () => events.forEach(event => this.off(event, cb))
  }

  once(event, listener) {
    if (!(event in this.listeners))
      this.listeners[event] = {}

    const cb = payload => {
      this.off(event, cb)
      listener(payload)
    }
    cb._id_ = ++this.nextId

    this.listeners[event][cb._id_] = cb

    return () => this.off(event, cb)
  }

  off(event, listener) {
    delete this.listeners[event][listener._id_]
  }
}

const eventEmitter = new EventEmitter()

const [
  RUNNING,
  CANCELLED,
  ABORTED,
  DONE,
  private,
] = [
  Symbol.for('SAGA-RUNNING'),
  Symbol.for('SAGA-CANCELLED'),
  Symbol.for('SAGA-ABORTED'),
  Symbol.for('SAGA-DONE'),
  Symbol.for('SAGA-private'),
]

let genId = (function *() {
  let id = 0
  while (true) {
    yield ++id
  }
})()

const doneResult = { done: true }

function _run(saga, parent, ...args) {
  let taskId = `SAGA_TASK_${genId.next().value}`
  let status = RUNNING

  let subTasks = {}
  let connections = []
  let finished = false
  let taskResult

  const task = {
    taskId,
    isRunning() {
      return status === RUNNING
    },
    isCancelled() {
      return status === CANCELLED
    },
    status() {
      return status
    },
    result() {
      return taskResult
    },
    [private]: {
      start() {
        Qt.callLater(advance)
      },
      deleteSubTask(taskId) {
        delete subTasks[taskId]
      },
      abort(e) {
        advance(ABORTED, e)
      },
      cancel() {
        if (finished) return
        advance(CANCELLED)
      }
    }
  }

  const it = saga(...args)
  function advance(response, error) {
    let action = 'next'

    if (response === CANCELLED) {
      status = CANCELLED
      action = 'return'
      connections.forEach(clear => clear())
      Object.values(subTasks).forEach(task => task[private].cancel())
      response = undefined
    }

    if (response === ABORTED) {
      action = 'throw'
      response = error
    }

    let result

    try {
      result = it[action](response)
    } catch(e) {
      if (status !== ABORTED) {
        status = ABORTED
        connections.forEach(clear => clear())
        if (parent) parent[private].abort(e)
        result = doneResult
      }
    }

    if (!result.done) {
      const effect = result.value

      switch (effect.type) {
      case 'call':
        effect.fn(...effect.args, () => {
          if(status === RUNNING)
            advance()
        })
        break;

      case 'fork':
        const subTask = _run(effect.saga, task, ...effect.args)
        subTasks[subTask.taskId] = subTask
        subTask[private].start()
        advance(subTask)
        break;

      case 'take':
        const ctake = eventEmitter.once(effect.action, advance)
        connections.push(ctake)
        break;

      case 'put':
        const cput = eventEmitter.emit(effect.action.type, effect.action)
        connections.push(cput)
        advance()
        break;

      case 'cancel':
        advance(CANCELLED)
        break;

      case 'cancelled':
        advance(status === CANCELLED)
        break;

      case 'join':
        if (effect.task.isRunning()) {
          eventEmitter.once(`${effect.task.taskId}_END`, advance)
        } else {
          advance(effect.task.status())
        }
        break;

      case 'race':
        const tasks = effect.effects.map(effect => {
          const subTask = _run(effect.fn, task, ...effect.args)
          subTasks[subTask.taskId] = subTask
          subTask[private].start()
          return subTask;
        })

        const events = tasks.map(task => `${task.taskId}_RESULT`)
        eventEmitter.race(events, payload => {
          tasks.forEach(task => task[private].cancel())
          advance(payload)
        })
      break;
      case 'all':
//        const tasks = effect.effects.map(effect => {
//          const subTask = _run(effect.fn, task, ...effect.args)
//          subTasks[subTask.taskId] = subTask
//          subTask[private].start()
//          return subTask;
//        })

//        const events = tasks.map(task => `${task.taskId}_RESULT`)
//        eventEmitter.all(events, payload => {
//          advance(payload)
//        })
      }
    } else {
      finished = true
      connections = []
      taskResult = result.value
      if (status === RUNNING) {
        status = DONE
        eventEmitter.emit(`${taskId}_RESULT`, taskResult)
      }

      eventEmitter.emit(`${taskId}_END`, status)

      if (parent) {
        parent[private].deleteSubTask(taskId)
      }
    }
  }

  return task
}

//-----------------//
function createT(n = 0) {
  return function *t() {
    try {
      yield delay(3000)
      return 'SALUDOS'
    } catch (e) {
      console.log(`T${n} ${e.message}`)
      throw e
    } finally {
      if (yield cancelled()) {
        console.log(`C${n}`)
      }
    }
  }
}

function createFn(name) {
  return function *() {
    try {
      for (let i = 0; i < 100; ++i) {
        yield delay(Math.random() * 10)
      }
      console.log(name, 'finished')
      return name
    } finally {
      if (yield cancelled()) {
        console.log(name, 'cancelled')
      }
    }
  }
}


function *test() {
  try {
    const effects = Array
      .from({ length: 2 },
            (_, i) => call(createFn(`function ${i}`))
      )
    const result = yield race(effects)

    console.log(result.find((r, i) => r && [r, i]))
  } catch (e) {
    console.log(`TEST: ${e.message}`)
  } finally {
    if (yield cancelled()) {
      console.log('TEST CANCELLED')
    }
  }
}

function run(saga, ...args) {
  const task = _run(saga, undefined, ...args)
  task[private].start()

  return task
}

