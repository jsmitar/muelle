import QtQuick 2.11
import QtQuick.Window 2.11
import QtQuick.Layouts 1.1
import org.duckdock 1.0

Window {
  id: window
  visible: true
  width: iconSize * 16 + iconZoom / 2
  x: 100
  y: 500

  height: iconZoom
  title: qsTr("DuckDock")

  property real iconFactor: 1.5
  property int iconSize: 48
  property int iconZoom: 87
  property int iconZoomAmplitude: iconSize / 5

  function randColor() {
    return Qt.rgba(Math.random(), Math.random(), Math.random(), 1)
  }

  function _each(collection, iteratee) {
    for (var i = 0; i < collection.length; i++)
      iteratee(collection[i], i, collection)

    return collection
  }

  function _filter(collection, predicate) {
    var filtered = []
    for (var i = 0; i < collection.length; i++)
      if (predicate(collection[i], i, collection))
        filtered.push(collection[i])

    return filtered
  }

  function _gen(size, iteratee) {
    var gen = []

    for (var i = 0; i < size; i++)
      gen.push(iteratee(i))

    return gen
  }

  MouseArea {
    id: dockArea
    hoverEnabled: true
    acceptedButtons: Qt.NoButton
    width: window.width
    height: window.height

    signal disableInitialAnimations
    signal enabledInitialAnimations

    property var tasksItems: content.children
    property real mousePosition: 0
    property int taskHovered: 0
    property var siblings: function (task) {
      return _filter([tasks.itemAt(task.index - 1), tasks.itemAt(
                        task.index + 1)], function (t) {
                          return t
                        })
    }

    onExited: {
      _each(tasksItems, function (task) {
        if (task._size)
          task._size = iconSize
      })
    }

    RowLayout {
      id: content
      spacing: -1

      anchors.bottom: parent.bottom
      state: dockArea.tasksItems.length / 2 > dockArea.taskHovered ? 'anchorLeft' : 'anchorRight'
      states: [
        State {
          name: 'anchorLeft'
          AnchorChanges {
            target: content
            anchors.left: parent.left
          }
        },
        State {
          name: 'anchorRight'
          AnchorChanges {
            target: content
            anchors.right: parent.right
          }
        }
      ]

      Repeater {
        id: tasks

        model: _gen(16, function (i) {
          return {
            id: i,
            index: i
          }
        })

        Icon {
          id: icon
          dock: dockArea
          Layout.minimumWidth: width
          Layout.minimumHeight: height
          Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
        }
      }
    }
  }
}
