import QtQuick 2.9
import QtQuick.Layouts 1.3
import org.muelle.types 1.0
import "../Extras"

Item {
  id: flex
  objectName: 'FlexSpacing'

  Layout.minimumWidth: Layout.preferredWidth
  Layout.minimumHeight: Layout.preferredHeight

  state: parent instanceof RowLayout ? 'row' : 'column'

  property bool fill: true
  property bool softFill: false

  property real _max: Layout.preferredWidth

  Behavior on _max {
    enabled: softFill
    NumberAnimation {
      easing.type: Easing.InOutQuad
      duration: store.state.animation.duration / 2
    }
  }

  PaintItem {
    target: flex
    enabled: true
  }

  states: [
    State {
      name: 'row'
      PropertyChanges {
        target: flex
        Layout.fillWidth: fill
        Layout.fillHeight: true
        Layout.maximumWidth: _max
        Layout.minimumWidth: _max
        Layout.maximumHeight: Layout.preferredHeight
        _max: Layout.preferredWidth
      }
    },
    State {
      name: 'column'
      PropertyChanges {
        target: flex
        Layout.fillWidth: true
        Layout.fillHeight: fill
        Layout.maximumWidth: Layout.preferredWidth
        Layout.maximumHeight: _max
        Layout.minimumHeight: _max
        _max: Layout.preferredHeight
      }
    }
  ]
}
