import QtQuick 2.12
import org.muelle.types 1.0
import '../Extras'
import 'state'

QObject {
  id: state
  objectName: '@State'

  readonly property Settings settings: Settings {}

  readonly property Panel panel: Panel {}

  readonly property Icon icon: Icon {
    iconSize: 48
    padding: 5
    spacing: 8
  }

  readonly property Geometry geometry: Geometry { state: state }

  readonly property Animation animation: Animation {
    duration: 800
  }
}
