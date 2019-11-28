.pragma library

(function (global){
var timerComponent = Qt.createComponent('./TimerComponent.qml')
var nextId = 0
var timers = {}

function clearTimeout(timerId) {
  if (!timers.hasOwnProperty(timerId)) {
    return
  }
  var timer = timers[timerId]
  timer.stop()
  timer.destroy()
  delete timers[timerId]
}

function setTimeout(callback, interval = 0) {
  var timer = timerComponent.createObject(null, { interval });

  timer.triggered.connect(function () {
    try {
      callback()
    } catch (e) {
      console.warn(e)
    } finally {
      timer.destroy()
      delete timers[timerId]
    }
  })
  var timerId = ++nextId
  timers[timerId] = timer
  timer.running = true

  return timerId
}

Qt.setTimeout = setTimeout
Qt.clearTimeout = clearTimeout
})(Qt)

