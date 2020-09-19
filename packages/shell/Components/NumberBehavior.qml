import QtQuick 2.14

Behavior {
  property int velocity: 2

  NumberAnimation {
    easing.type: Easing.OutQuad
    duration: store.state.animation.duration / velocity
  }
}