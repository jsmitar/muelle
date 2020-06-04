import QtQuick 2.14
import org.muelle.extra 1.0 as Muelle
import 'qrc:/shared/components'

QObject {
  objectName: '@State/Background'

  // padding in percent [0, 1]
  property real paddingX: 0

  // padding in percent [-1, 1]
  property real paddingTop: 0

  // padding in percent [0, 1]
  property real paddingBottom: 0

  // inset in px
  property int inset: 10

  property bool shadow: shadowBlur > 0

  property real shadowBlur: 0

  readonly property Muelle.Radius radius: Muelle.Radius {}

  // type Style = 'solid' | 'svg' | 'none'
  property string style: 'svg'

  readonly property bool visible: paddingTop > -1 
}
