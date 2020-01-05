import QtQuick 2.10
import QtGraphicalEffects 1.0
import '../../shared/functional.ts' as F

QObject {
  id: paintItem
  property Item target
  property bool enabled: false
  property bool showSize: true
  property bool _enableAll: false

  Component {
    id: bg

    Item {
      z: -1
      width: 0
      height: 0

      Rectangle {
        border.width: 1
        antialiasing: true
        border.color: '#99ffffff'
        radius: 2
        color: F.randColor(0.4)
        width: target.width
        height: target.height
        onWidthChanged: color = F.randColor(0.4)
        onHeightChanged: color = F.randColor(0.4)
      }
    }
  }

  Component {
    id: fg

    Item {
      z: 999
      width: 0
      height: 0
      visible: showSize
      x: Math.max((parent.width - label.width), 0)
      y: Math.max((parent.height - label.height), 0)

      Rectangle {
        width: label.width
        height: label.height
        radius: height
        color: "#90000000"
        objectName: "PaintItem-Label"

        Text {
          id: label
          text: target.width === target.height ? target.width : `${target.width | 0}x${target.height | 0}`
          horizontalAlignment: Text.AlignHCenter
          verticalAlignment: Text.AlignVCenter
          font.pointSize: 9
          color: "#ddd"
          leftPadding: 3
          rightPadding: 3
        }
      }
    }
  }

  Loader {
    sourceComponent: paintItem.enabled && _enableAll && target ? bg : undefined

    onLoaded: {
      item.parent = target
    }
  }

  Loader {
    sourceComponent: paintItem.enabled && _enableAll && target && showSize ? fg : undefined

    onLoaded: {
      item.parent = target
    }
  }
}
