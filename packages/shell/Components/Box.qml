import QtQuick 2.14
import 'qrc:/shared/functional.ts' as F
import "Private"

Item {
  id: box
  objectName: 'Box'

  default property alias content: paddingBox.children

  property real margins: 0
  readonly property BoxSideProps margin: BoxSideProps {}

  property real paddings: 0
  readonly property BoxSideProps padding: BoxSideProps {}

  anchors {
    topMargin: margin.top || margin.y || margins
    rightMargin: margin.right || margin.x || margins
    bottomMargin: margin.bottom || margin.y || margins
    leftMargin: margin.left || margin.x || margins
    verticalCenterOffset: margin.y
    horizontalCenterOffset: margin.x
  }

  Item {
    id: paddingBox
    anchors {
      fill: box
      topMargin: padding.top || padding.y || paddings
      rightMargin: padding.right || padding.x || paddings
      bottomMargin: padding.bottom || padding.y || paddings
      leftMargin: padding.left || padding.x || paddings
    }

    onVisibleChildrenChanged: {
      const child = visibleChildren[0]
      if (child)
        child.anchors.fill = Qt.binding(() => paddingBox)
    }
  }
}
