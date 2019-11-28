import QtQuick 2.12
import "../libs/functional.js" as F
import "Private"

Item {
  id: box

  default property alias content: innerContent.children

  property real margins: 0
  readonly property BoxSideProps margin: BoxSideProps {
  }

  property real paddings: 0
  readonly property BoxSideProps padding: BoxSideProps {
  }

  anchors {
    topMargin: margins || margin.top || margin.y
    rightMargin: margins || margin.right || margin.x
    bottomMargin: margins || margin.bottom || margin.y
    leftMargin: margins || margin.left || margin.x
  }

  Item {
    id: innerContent
    anchors {
      fill: box
      topMargin: paddings || padding.top || padding.y
      rightMargin: paddings || padding.right || padding.x
      bottomMargin: paddings || padding.bottom || padding.y
      leftMargin: paddings || padding.left || padding.x
    }

    onChildrenChanged: {
      if (children[0])
        children[0].anchors.fill = Qt.binding(() => innerContent)
    }
  }
}
