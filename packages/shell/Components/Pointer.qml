import QtQuick 2.14
import QtGraphicalEffects 1.14

Item {
  id: pointer
  width: 5
  height: 5

  property color color: '#000000'
  property bool glow: false

  RectangularGlow {
    id: effect
    anchors.fill: rect
    glowRadius: 8
    spread: 0.1
    color: rect.color
    cornerRadius: rect.radius + glowRadius

    visible: scale > 0
    scale: glow ? 1 : 0
    Behavior on scale {
      NumberAnimation {
        target: effect
        property: 'scale'
        duration: store.state.animation.duration
      }
    }
  }

  Rectangle {
    id: rect
    anchors.fill: parent
    radius: width
    color: pointer.color
  }
}