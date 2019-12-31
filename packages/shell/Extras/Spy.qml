import QtQuick 2.12
import QtQml 2.12
import "../libs/functional.js" as F

QObject {
  id: spy
  property var properties: spy``
  property bool delayed: false
  property bool _disableAll: true

  function spy(strings, ...targets) {
    const properties = strings
      .slice(1)
      .map(str => str.replace(/[\.\(\)\s]+/g, '').split(','))

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
          const parent = `${targets.target.parent || ''}`.replace(/[_(].*/g, '')
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
