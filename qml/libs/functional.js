// disable-format
.pragma library

.import QtQml 2.3 as Qml

function randColor(alpha) {
  alpha = alpha || 1
  return Qt.rgba(0.5 + Math.random() / 2, 0.5 + Math.random() / 2,
                 0.5 + Math.random() / 2, alpha)
}

function alpha(color, alpha) {
  return Qt.rgba(color.r, color.g, color.b, alpha)
}

function clamp(min, value, max) {
  return value > max ? max : value < min ? min : value
}

function identity(item) {
  return item
}

// NoOperation
function noop() {}

function each(collection, iteratee) {
  return Array.prototype.forEach.call(collection, iteratee)
}

function map(collection, iteratee) {
  return Array.prototype.map.call(collection, iteratee)
}

function reduce(collection, iteratee, initial) {
  return Array.prototype.reduce.call(collection, iteratee, initial)
}

function filter(collection, predicate) {
  return Array.prototype.filter.call(collection, predicate)
}

function intersect(collectionL, collectionR) {
  return Array.prototype.filter(collectionL, item => {
    return Array.prototype.includes(collectionR, item)
  })
}

function until(collection, predicate) {
  var filtered = []

  predicate = predicate || no
  for (var i = 0; i < collection.length; i++) {
    filtered.push(collection[i])
    if (predicate(collection[i], i, collection))
      break
  }

  return filtered
}

function range(start = 0, stop = undefined, step = 1) {
  if (stop === undefined) {
    stop = start
    start = 0
  }

  return {
    *[Symbol.iterator]() {
      for (let i = start; i !== stop; i += step)
        yield i
    }
  }
}

function zoom(iconSize, iconZoom, iconZoomAmplitude) {
  // precalculated
  var X = iconZoom - iconSize - iconZoomAmplitude
  var A = iconZoom / 2
  var BASE = iconSize + iconZoomAmplitude

  function calcZoom(position) {
    return clamp(iconSize, BASE + (position * X) / A, iconZoom)
  }

  return calcZoom
}

function timeout(timer, cb, interval) {
  function disconnect() {
    if (timer && timer.triggered) {
      timer.triggered.disconnect(cb)
      timer.stop()
    }
  }

  timer.stop()
  timer.interval = interval !== undefined ? interval : timer.interval
  timer.triggered.connect(cb)
  timer.triggered.connect(disconnect)
  timer.start()
  return disconnect
}

function asyncTimeout(interval) {
  return new Qt.Promise(r => Qt.setTimeout(r, interval))
}

function yes() {
  return true
}

function no() {
  return false
}

function isString(value) {
  return typeof value === 'string' || value instanceof String;
}

function isNumber(value) {
  return typeof value === 'number' || value instanceof Number;
}

function isCallable(value) {
  return typeof value === 'function'
}

function isObject(value) {
  return value && typeof value === 'object';
}

function isArray(value) {
  return Array.isArray(value);
}

function quote(str) {
  return `"${str}"`;
}

function space(length) {
  return repeat(' ', length)
}

function formatHelper(str, [left, right], indent) {
  return `${left}\n${space(indent)}${str}\n${space(indent - 2)}${right}`
}


function atostr(array, deep = 5, indent = 2) {
  if (deep === 0) {
    return '[Array]'
  }

  if (!array.length) {
    return '[]';
  }

  const str = array
    .map(v => `${tostr(v, deep - 1, indent + 2)}`)
    .join(`,\n${space(indent)}`)

  return formatHelper(str, ['[', ']'], indent);
}

function otostr(object, deep = 5, indent = 2) {
  if (deep === 0) {
    const ostr = String(object) || 'Object'
    return `[object ${ostr}${object.objectName ? '-' + object.objectName: ''}]`
  }

  if (!Object.keys(object).length) {
    return '{}';
  }

  const str = Object
    .entries(object)
    .filter(([k]) => !k.startsWith("_"))
    .map(([k, v]) => {
      let nextDeep = deep - 1
      if (k === 'parent' || object[k] === object.parent || v === object) {
        nextDeep = 0
      }
      return `${quote(k)}: ${tostr(v, nextDeep, indent + 2)}`
    })
    .join(`,\n${space(indent)}`);

  return formatHelper(str, ['{', '}'], indent);
}

function ctostr(callable) {
  if (callable.name) {
    return `[Function: ${callable.name}]`
  }
  return '[Function]'
}

function repeat(value, length = 1) {
  return Array.from({ length: length + 1 }).join(value)
}

function tostr(value, deep = 5, indent = 2) {
  if (isString(value)) {
    return quote(value);
  }
  if (isNumber(value)) {
    return Number(value.toFixed(3))
  }
  if (isArray(value)) {
    return atostr(value, deep, indent);
  }
  if (isCallable(value)) {
    return ctostr(value);
  }
  if (isObject(value)) {
    return otostr(value, deep, indent);
  }
  if (!value) {
    return value
  }

  return String(value)
}

function singleConnect(csignal, slot) {
  try {
    csignal.connect(function _slot() {
      csignal.disconnect(_slot)
      slot.apply(null, arguments)
    })
  } catch (e) {
    console.warn(e)
  }
}

/**
  * @returns {disconnect: Function}
  */
function on(signal, slot) {
  try {
    signal.connect(slot);
    return () => signal.disconnect(slot)
  } catch (e) {
    console.warn(e)
  }
}

/**
  * @returns {[]}
  * args args (n-1) are signals
  * slot: last argument is considered a slot
  */
function once(...args) {
  if (args.length < 2) throw new Error('insufficient args')

  try {
    const signals = args.slice(0, -1)
    const slot = args[args.length - 1]

    if (slot) {
      const clear = () =>
        signals.forEach(signal => signal.disconnect(slot))

      signals.forEach(signal => {
        signal.connect(slot); signal.connect(clear)
      })
      return slot
    }
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

    if (slot)
      signals.forEach(signal => signal.disconnect(slot))

  } catch(e) {
    console.warn(e)
  }
}

function assign(target, ...values) {
  each(values, value => {
    Object.entries(value).forEach(([k, v]) => {
      if(isObject(v)) {
        assign(target[k], v)
      } else {
        target[k] = v
      }
    })
  })
}

function invariant(condition, format, ...args) {
  if (!format) {
    throw new Error('format is undefined')
  }

  if (!condition) {
    let index = 0
    const error = new Error(format.replace(/%s/g, () => args[index++]))
    error.name = 'Invariant violation';
    throw error
  }
}

const _filter = filter
const _map = map
const _reduce = reduce
const _each = each
const _until = until
const _intersect = intersect

class Iter {
  constructor(iter) {
    this.iter = iter
  }

  get value() {
    return this.iter
  }

  static from(value) {
    if (isArray(value)) {
      return new Iter(value)
    }

    if (isObject(value)) {
      return new Iter(Object.values(value))
    }

    throw new Error('Iter.from: value is not an iterable')
  }

  filter(predicate) {
    this.iter = _filter(this.iter, predicate)
    return this
  }

  map(iteratee) {
    this.iter = _map(this.iter, iteratee)
    return this
  }

  foldl(iteratee, initial) {
    this.iter = _reduce(this.iter, iteratee, initial)
    return this
  }

  each(iteratee) {
    _each(this.iter, iteratee)
    return this
  }

  take(limit) {
    this.iter = this.iter.slice(0, limit)
    return this
  }

  until(predicate) {
    this.iter = _until(this.iter, predicate)
    return this
  }

  reverse() {
    this.iter.reverse()
    return this
  }

  intersect(collection) {
    this.iter = _intersect(this.iter, collection)
    return this
  }
}
