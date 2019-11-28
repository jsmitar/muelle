import QtQuick 2.0

QtObject {
  id: bindingValue
  property var target
  property var initial

  // trigger prop
  property string when

  // (target: Item | QtObject) => void
  property var then
  property var otherwise: () => {}

  Component.onCompleted: {
    target[when] = initial
    //initial = initial
    target[`${when}Changed`].connect(
      () => target[when] ? then(target) : otherwise(target)
    )
  }
}
