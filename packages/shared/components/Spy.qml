import QtQuick 2.12
import QtQml 2.12
import '../../shared/functional.ts' as F

QObject {
  id: spy
  property var properties: spy``
  property bool delayed: false
  property bool _disableAll: false

  function spy(strings, ...targets) {
    const properties = strings
      .map(str => str.replace(/[\.\(\)\s]+/g, '').split(','))

    if (targets.length === 0) {
      targets = [parentObject]
    }
    return F.zip(targets, properties)
  }

  Instantiator {
    active: !_disableAll
    model: F.flat(properties)

    Instantiator {
      id: targets
      model: modelData[1]
      property var target: modelData[0]

      QObject {
        property var property: targets.target[modelData]

        function printProperty() {
          const targetName = `${targets.target}`.replace(/[_(].*/g, '')
          const parent = `${targets.target.parentObject || ''}`.replace(/[_(].*/g, '')
          const propName = `${parent ? parent + '.' : ''}${targetName}.${modelData}`
          console.log(`${propName}: ${F.tostr(property, 1, -Infinity)}`)
        }

        onPropertyChanged: {
          if (spy.delayed) {
            Qt.callLater(printProperty)
          } else {
            printProperty()
          }
        }
      }
    }
  }
}
