import QtQuick 2.14
import 'qrc:/shared/components'
import 'qrc:/shell/Store/actions.ts' as Action

QObject {
  id: slide
  property Item target

  property int value: -distance 
  property int distance: store.state.geometry.view.height
  property int delay: 0

  signal slideInFinished
  signal slideOutFinished

  property bool enabled: true

  Connections {
    enabled: slide.enabled
    target: store.state.panel

    function onSlideChanged() {
      const slide = target.slide
      if (slide === 'slide-in-running') {
        if (delay)
          Qt.setTimeout(() => slideIn.start(), delay)
        else
          slideIn.start()
      } else if (slide === 'slide-out-running') {
        if (delay)
          Qt.setTimeout(() => slideOut.start(), delay)
        else
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
