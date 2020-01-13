import QtQuick 2.12
import '../../Extras'

QObject {
  objectName: '@State/Animation'
  property int duration: 2000

  // units/second
  property int velocity: (120 * 1000) / duration
}
