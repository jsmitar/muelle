import QtQuick 2.12
import "../../../Extras"

QObject {
  property int duration: 0

  // units/second
  property int velocity: (120 * 1000) / duration
}
