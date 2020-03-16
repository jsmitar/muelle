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
    color: '#2D343C'
    border.width: 0
    radius: 2
    opacity: 0.95
    
    anchors.fill: parent
  }
}