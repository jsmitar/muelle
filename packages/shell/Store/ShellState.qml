import QtQuick 2.12
import org.muelle.types 1.0
import '../../shared/components'
import 'state'

QObject {
  id: state
  objectName: '@State'

  readonly property Settings settings: Settings { parentObject: state }

  readonly property Panel panel: Panel { parentObject: state }

  readonly property Icon icon: Icon {
    parentObject: state
    iconSize: 48
    padding: 5
    spacing: 8
  }

  readonly property Geometry geometry: Geometry { 
    parentObject: state
    state: state 
  }

  readonly property Animation animation: Animation {
    parentObject: state
    duration: 800
  }
}
