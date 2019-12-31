import QtQuick 2.0

Object {
  id: bindModel
  property var value

  signal changed(var value)

  onChanged: {
    if (value != bindModel.value) {
      bindModel.value = value
    }
  }

}
