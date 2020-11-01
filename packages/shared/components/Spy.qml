import QtQuick 2.14
import QtQml 2.12
import 'qrc:/shared/functional.ts' as F

QObject {
  id: spy
  property var properties: spy``
  property bool _disableAll: false
  property string name: ''

  function spy(strings, ...targets) {
    const properties = strings
      .filter(str => str)
      .map(str => str.replace(/[\.\(\)\s]+/g, '').split(':'))

    if (targets.length === 0) {
      targets = [parentObject]
    }

    return F.zip(targets, properties)
  }

  Instantiator {
    active: !_disableAll
    model: properties

    Instantiator {
      id: targets
      model: modelData[1]
      readonly property var target: modelData[0]

      QObject {
        readonly property var property: targets.target[modelData]

        function printProperty() {
          const targetName = targets.target.objectName || `${targets.target}`.replace(/[_(].*/g, '')
          const parent = `${targets.target.parentObject || ''}`.replace(/[_(].*/g, '')
          const propName = `${parent ? parent + '.' : ''}${targetName}.${modelData}`
          console.log(`${name ? name + ' ' : ''}${propName}: ${F.tostr(property, 1, -1)}`)
        }

        onPropertyChanged: printProperty()
      }
    }
  }
}


