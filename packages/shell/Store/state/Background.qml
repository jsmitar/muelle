import QtQuick 2.12
import '../../../shared/components'

QObject {
  objectName: '@State/Background'

  property int paddingX: 10

  property int paddingY: 5

  property int lift: 10

  property int borderRadius: 0

  property bool shadow: true

  property real shadowBlur: 10

  readonly property bool visible: lift > 0 
}
