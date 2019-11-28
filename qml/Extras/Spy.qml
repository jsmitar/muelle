import QtQuick 2.12
import QtQml 2.12

QObject {
  id: spy
  property QtObject target
  property string properties: ""

  function getIn(modelData) {
    return modelData
      .split('.')
      .reduce((target, prop) => target[prop], target)
  }

  Instantiator {
    active: target
    model: properties.replace(/\s/g, '').split(",")

    delegate: QtObject {
      property var property: getIn(modelData)

      onPropertyChanged: {
        console.log(`[SPY] ${modelData}:`, property)
      }
    }
  }
}
