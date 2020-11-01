import QtQuick 2.14
import 'qrc:/shared/functional.ts' as F
import org.kde.plasma.core 2.0 as PlasmaCore

Rectangle {
  id: badge

  property alias label: _label.text

  readonly property int m: Math.max(
    theme.mSize().width, 
    theme.mSize().height
  ) - 2

  width: Math.max(m - m % 2, _label.contentWidth + 4)
  height: m - m % 2
  radius: height
  color: '#F23C34'

  Text {
    id: _label
    
    anchors.centerIn: badge

    Behavior on text {
      SequentialAnimation {
        NumberAnimation {
          target: badge
          property: 'scale'
          to: 1.4
          duration: store.state.animation.duration / 4
        }
        NumberAnimation {
          target: badge
          property: 'scale'
          to: 1
          duration: store.state.animation.duration / 4
        }
      }
    }

    color: '#CFD8DC'
    horizontalAlignment: Text.AlignLeft
    verticalAlignment: Text.AlignVCenter

    font {
      weight: Font.Black
      bold: true
      pointSize: 10
    }
  }
}