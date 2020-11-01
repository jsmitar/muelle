import QtQuick 2.14
import 'qrc:/shared/components'

QObject {
  objectName: '@State/Animation'

  property int duration: 0

  readonly property int shortDuration: duration / 2.5

  // units/second
  readonly property int velocity: (120 * 1000) / duration
}
