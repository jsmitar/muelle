.pragma library

(function (global){
const timerComponent = Qt.createComponent('./TimerComponent.qml')
const timers = {}
let nextId = 0

function clearTimeout(id) {
  if (!timers.hasOwnProperty(id)) return

  const timer = timers[id]
  timer.stop()
  timer.destroy()
  delete timers[id]
}

function setTimeout(callback, interval = 0) {
  const timer = timerComponent.createObject(null, { interval, repeat: false });
  const id = ++nextId
  timers[id] = timer

  timer.triggered.connect(() => {
    try {
      callback()
    } catch (e) {
      console.warn(e)
    } finally {
      timer.destroy()
      delete timers[id]
    }
  })
  timer.running = true

  return id
}

function setInterval(callback, interval = 0) {
  const timer = timerComponent.createObject(null, { interval, repeat: true });
  const id = ++nextId
  timers[id] = timer

  timer.triggered.connect(() => {
    try {
      callback()
    } catch (e) {
      console.warn(e)
    }
  })
  timer.running = true

  return id
}

Qt.setTimeout = setTimeout
Qt.clearTimeout = clearTimeout

Qt.setInterval = setInterval
Qt.clearInterval = clearTimeout

})(Qt)

