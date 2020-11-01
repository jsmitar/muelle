import QtQuick 2.14
import 'qrc:/shared/functional.ts' as F
import org.kde.plasma.core 2.0 as PlasmaCore
import QtGraphicalEffects 1.14

Rectangle {
  id: bg

  property bool active: false
  property real value: 0

  readonly property bool indeterminate: value <= 0

  property real _from: 0
  property real _to: 0

  layer.enabled: true
  layer.effect: OpacityMask {
    anchors.fill: bg
    maskSource: Rectangle {
      width: bg.width
      height: bg.height
      radius: height
      visible: false
    }
  }

  visible: active
  color: '#40828282'

  Binding {
    when: !bg.indeterminate
    target: bg
    property: '_to'
    value: bg.value
  }
  Binding {
    when: !bg.indeterminate
    target: bg
    property: '_from'
    value: 0
  }

  ParallelAnimation {
    running: active && indeterminate
    loops: Animation.Infinite
    alwaysRunToEnd: false

    SequentialAnimation {
      NumberAnimation {
        target: bg
        property: '_from'
        from: 0
        to: 0.85
        duration: 500
        easing.type: Easing.InSine
      }
      NumberAnimation {
        target: bg
        property: '_from'
        to: 0
        from: 0.85
        duration: 500
        easing.type: Easing.OutSine
      }
    }
    SequentialAnimation {
      NumberAnimation { 
        target: bg
        property: '_to'
        from: 0.15
        to: 1
        duration: 500
        easing.type: Easing.OutSine
      }
      NumberAnimation { 
        target: bg
        property: '_to'
        to: 0.15
        from: 1
        duration: 500
        easing.type: Easing.InSine
      }
    }
  }

  Rectangle {
    id: progress
    color: '#2D9CDB'
    radius: indeterminate ? height : 0
    x: -height + bg.width * bg._from
    width: height + (bg.width + 2) * Math.abs(bg._from - bg._to)
    height: bg.height
  }
}