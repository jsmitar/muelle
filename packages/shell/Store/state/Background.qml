import QtQuick 2.14
import '../../../shared/components'

QObject {
  objectName: '@State/Background'

  property int paddingX: 0

  property int paddingY: 0

  property int lift: 0

  property int borderRadius: 0

  property bool shadow: true

  property real shadowBlur: 0

  // type Style = 'solid' | 'svg' | 'none'
  property string style: 'svg'

  readonly property bool visible: lift > 0 
}
