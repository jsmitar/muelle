import QtQuick 2.12
import org.muelle.extra 1.0
import '../../shared/functional.ts' as F

QtObject {
  id: bindingValue

  property QtObject target
  property var initial
  property string property

  // (target: QtObject) => void
  property var then: F.noop
  property var otherwise: F.noop

  signal valueChanged

  onValueChanged: {
    const value = target[property]
    if (value) then(value, target)
    else otherwise(value, target)
  }

  Component.onCompleted: {
    if (initial) target[property] = initial
    target[`${property}Changed`].connect(valueChanged)
  }

  Component.onDestruction: {
    target[`${property}Changed`].disconnect(valueChanged)
  }
}
