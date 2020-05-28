import QtQuick 2.14
import '../../../shared/components'

QObject {
  objectName: '@State/Icon'

  readonly property int size: iconSize + padding * 2

  property int iconSize: 0

  property int spacing: 0

  property int padding: 0

  readonly property int separator: 1 + padding * 2
}
