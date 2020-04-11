import QtQuick 2.12
import '../../shared/components'
import '../Store/actions.ts' as Action

QObject {
  id: slide
  property Item target

  property real value: -distance 
  property real distance: 0

  signal slideInFinished
  signal slideOutFinished

  property bool enabled: true

  Connections {
    enabled: slide.enabled
    target: store.state.panel

    onSlideChanged: {
      const slide = target.slide
      if (slide === 'slide-in-running') {
        slideIn.start()
      } else if (slide === 'slide-out-running') {
        slideOut.start()
      }
    }
  }

  SequentialAnimation {
    id: slideIn
    alwaysRunToEnd: false

    ScriptAction {
      script: slideOut.stop()
    }
    SmoothedAnimation {
      target: slide
      property: 'value'
      velocity: store.state.animation.velocity
      easing.type: Easing.OutCubic
      to: 0
    }
    ScriptAction {
      script: slide.slideInFinished()
    }
  }

  SequentialAnimation {
    id: slideOut
    alwaysRunToEnd: false

    ScriptAction {
      script: slideIn.stop()
    }
    SmoothedAnimation {
      target: slide
      property: 'value'
      velocity: store.state.animation.velocity
      easing.type: Easing.InCubic
      to: -distance
    }
    ScriptAction {
      script: slide.slideOutFinished()
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
