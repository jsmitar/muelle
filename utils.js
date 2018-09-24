function randColor(alpha) {
  alpha = alpha || 1
  return Qt.rgba(Math.random(), Math.random(), Math.random(), 1)
}

function boundary(min, value, max) {
  return value > max ? max : value < min ? min : value
}

function each(collection, iteratee) {
  for (var i = 0; i < collection.length; i++)
    iteratee(collection[i], i, collection)

  return collection
}

function filter(collection, predicate) {
  var filtered = []
  for (var i = 0; i < collection.length; i++)
    if (predicate(collection[i], i, collection))
      filtered.push(collection[i])

  return filtered
}

function gen(size, iteratee) {
  var gen = []

  for (var i = 0; i < size; i++)
    gen.push(iteratee(i))

  return gen
}

function zoom(iconSize, iconZoom, iconZoomAmplitude) {
  // precalculated
  var X = iconZoom - iconSize - iconZoomAmplitude
  var A = iconZoom / 2
  var BASE = iconSize + iconZoomAmplitude

  function calcZoom(position) {
    return boundary(iconSize, BASE + position * X / A, iconZoom)
  }

  return calcZoom
}

function timeout(timer, cb, interval) {
  timer.interval = interval !== undefined ? interval : timer.interval
  timer.triggered.connect(cb)
  timer.triggered.connect(function () {
    timer.triggered.disconnect(cb)
  })
  timer.start()
}
