.pragma library

class PropsRegExp extends RegExp {
  [Symbol.split](str, limit) {
    return RegExp.prototype[Symbol.split].call(this, str, limit)
      .map(prop => prop.replace(/[\.\(\)\s]+/g, ''));
  }
}
