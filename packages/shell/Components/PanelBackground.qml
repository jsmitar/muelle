import QtQuick 2.12
import '../../shared/functional.ts' as F

Item {
  readonly property rect backgroundRect: store.state.geometry.backgroundRect

  x: backgroundRect.x
  y: backgroundRect.y
  width: backgroundRect.width
  height: backgroundRect.height

  Rectangle {
    antialiasing: true
    border.color: F.randColor()
    border.width: 0
    radius: 2
    
    anchors.fill: parent
  }
}