import QtQuick 2.12
import QtQuick.Layouts 1.3
import '../Extras'

QObject {
  id: slide

  property Item target

  // hide < 0
  // show == 0
  property real value: 0
  property real size: store.state.icon.size

  readonly property alias show: showAnim.control
  readonly property alias hide: hideAnim.control

  AnimationControl {
    id: hideAnim
    alwaysRunToEnd: false
    ScriptAction {
      script: slide.show.cancelAndStop()
    }
    PauseAnimation {
      duration: 50
    }
    SmoothedAnimation {
      target: slide
      property: 'value'
      velocity: store.state.animation.velocity
      easing.type: Easing.InCubic
      to: -size
    }
    PauseAnimation {
      duration: 50
    }
  }

  AnimationControl {
    id: showAnim
    alwaysRunToEnd: false
    ScriptAction {
      script: slide.hide.cancelAndStop()
    }
    PauseAnimation {
      duration: 50
    }
    SmoothedAnimation {
      target: slide
      property: 'value'
      velocity: store.state.animation.velocity
      easing.type: Easing.OutCubic
      to: 0
    }
    PauseAnimation {
      duration: 50
    }
  }

  StateLayoutEdge {
    reset: PropertyChanges {
      target: slide.target
      anchors {
        verticalCenterOffset: 0
        horizontalCenterOffset: 0
      }
    }
    top: PropertyChanges {
      target: slide.target
      anchors.verticalCenterOffset: slide.value
    }
    right: PropertyChanges {
      target: slide.target
      anchors.horizontalCenterOffset: -slide.value
    }
    bottom: PropertyChanges {
      target: slide.target
      anchors.verticalCenterOffset: -slide.value
    }
    left: PropertyChanges {
      target: slide.target
      anchors.horizontalCenterOffset: slide.value
    }
  }
}

