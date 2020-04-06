import QtQuick 2.12
import '../../../shared/components'

QObject {
  objectName: '@State/Animation'

  property int duration: 500

  // units/second
  property int velocity: (120 * 1000) / duration

  property bool addEnabled: true

  readonly property int edgeDistance: Math.max(
    parentObject.geometry.background.height,
    parentObject.geometry.panel.height
  ) 
}
