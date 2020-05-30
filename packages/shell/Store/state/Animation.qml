import QtQuick 2.14
import 'qrc:/shared/components'

QObject {
  objectName: '@State/Animation'

  property int duration: 500

  property int shortDuration: duration / 2.5

  // units/second
  property int velocity: (120 * 1000) / duration

  property bool addEnabled: true

  readonly property int edgeDistance: Math.max(
    parentObject.geometry.background.height + 
    parentObject.geometry.background.shadow,
    parentObject.geometry.panel.height
  ) 
}
