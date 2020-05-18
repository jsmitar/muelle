import QtQuick 2.12
import '../../shared/components'
import '../../shared/functional.ts' as F

QObject {
  property string objectName: ''
  property Item target
  property bool running: false

  onRunningChanged: {
    if (!running) return

    const animation = store.propertyBuffer[`${objectName}:startupAnimation`]

    if (animation && !animation.running) {
      F.countdown(
        animation.start,
        animation.stop, 
        store.state.animation.duration * 1.5
      )
    }
  }

  onObjectNameChanged: {
    const itemName = `${objectName}:startupAnimation`
    const buffer = store.propertyBuffer

    let animation = buffer[itemName]

    if (!animation) {
      animation = startupComponent.createObject(root, { objectName })
      buffer[itemName] = animation
    }

    target.rotation = Qt.binding(() => animation.rotation)
  }

  Component {
    id: startupComponent

    QObject {
      id: _target

      property string objectName
      property var running: rotationAnim.running
      property var start: rotationAnim.start

      property var stop: () => {
        rotationAnim.stop()
        restoreAnim.start()
      }
      property real rotation: 0

      SequentialAnimation {
        id: rotationAnim
        loops: Animation.Infinite

        RotationAnimation {
          target: _target
          to: -12
          easing.type: Easing.OutQuad
          duration: store.state.animation.shortDuration
        }
        RotationAnimation {
          target: _target
          to: 12
          easing.type: Easing.OutQuad
          duration: store.state.animation.shortDuration
        }
      }

      RotationAnimation {
        id: restoreAnim
        target: _target
        to: 0
        easing.type: Easing.OutQuad
        duration: store.state.animation.shortDuration
      }
    }
  }

}