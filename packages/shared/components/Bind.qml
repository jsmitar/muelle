import QtQuick 2.12

QObject {
  id: bind
  objectName: "Bind"

  property QtObject target
  property string property

  readonly property var value: target && property in target ? target[property] : undefined
  onValueChanged: parentObject && parentObject.propertyChanged(value)
}
