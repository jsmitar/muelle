import QtQuick 2.14

QtObject {
  objectName: 'ConnectionGroup'
  property bool enabled: true

  default property list<Connections> resources

  onResourcesChanged: {
    Array.prototype.forEach.call(resources, c => {
      c.enabled = Qt.binding(() => enabled)
    })
  }
}
