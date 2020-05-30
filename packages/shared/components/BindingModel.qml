import QtQuick 2.14
import 'qrc:/shared/functional.ts' as F

QObject {
  id: model

  property var initial: undefined

  // property Spy _: Spy {
  //   properties: [spy`${model}.(initial, prevValue, resources)`]
  // }

  property var prevValue: undefined

  signal propertyChanged(var nextValue)

  function update(nextValue) {
    if (nextValue != prevValue) {
      prevValue = nextValue
      F.each(resources, ({ target, property, value }) => {
        if (value !== nextValue) target[property] = nextValue
      })
    }
  }

  onResourcesChanged: {
    if (initial !== undefined) {
      initial = initial
      update(initial)
      propertyChanged.connect(F.debounce(update))
    } else {
      F.once(initialChanged, () => update(initial))
    }
  }

}
