import QtQuick 2.12
import '../../shared/components'
import '../../shared/functional.ts' as F

QObject {
  property string objectName: ''
  property Item target
  property bool running: false

  onRunningChanged: {
    if (running) {
      const animation = store.propertyBuffer[`${objectName}:startupAnimation`]
      animation && animation.start()
    }
  }

  onObjectNameChanged: {
    const buffer = store.propertyBuffer

    const itemName = `${objectName}:startupAnimation`
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

      property var start: rotationAnim.start

      property real rotation: 0

      property real duration: store.state.animation.shortDuration / 2

      SequentialAnimation {
        id: rotationAnim
        loops: 2

        RotationAnimation { 
          target: _target
          to: -10
          easing.type: Easing.OutSine
          duration: target.duration
        }
        RotationAnimation { 
          target: _target
          to: 0
          easing.type: Easing.InSine
          duration: target.duration / 2
        }
        RotationAnimation { 
          target: _target
          to: 10
          easing.type: Easing.OutSine
          duration: target.duration
        }
        RotationAnimation { 
          target: _target
          to: 0
          easing.type: Easing.InSine
          duration: target.duration / 2
        }
      }
    }
  }
}