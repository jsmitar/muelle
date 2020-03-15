import QtQuick 2.12
import '../../../shared/components'

QObject {
  objectName: '@State/Icon'

  readonly property int size: iconSize + padding * 2

  property int iconSize: 0

  property int spacing: 0

  property int padding: 0

  property int separator: 1
}
