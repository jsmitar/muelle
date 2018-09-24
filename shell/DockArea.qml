import QtQuick 2.11
import QtQuick.Layouts 1.1
import org.duckdock.types 1.0
import "qrc:/utils.js" as F

Rectangle {
  id: panel

  color: "#e7e8eb"

  signal disableInitialAnimations
  signal enabledInitialAnimations

  property var taskItems: []
  property real mousePosition: 0
  property int taskHovered: -1
  property int offset: 0

  SmoothedAnimation on offset {
    id: aOffset
    running: false
    duration: 100
    to: 0
  }

  property var siblings: function _siblings(task) {
    return {
      l0: taskItems[task.index - 1],
      c: taskItems[task.index],
      r0: taskItems[task.index + 1]
    }
  }

  Connections {
    target: view
    onExited: {
      var lastHovered = taskHovered
      taskHovered = -1
      aOffset.start()
      F.each(taskItems, function (task) {
        task.animation.enabled = Math.abs(task.index - lastHovered) <= 1
        task._size = iconSize
      })
    }
  }

  Binding {
    target: panel
    property: 'taskItems'
    when: loader.status === Loader.Ready
    value: F.filter(loader.item.children, function (i) {
      return i.objectName === 'icon'
    })
  }

  Loader {
    id: loader

    states: [
      State {
        when: layout.orientation === DockType.Horizontal
        AnchorChanges {
          target: loader
          anchors.horizontalCenter: panel.horizontalCenter
        }
        PropertyChanges {
          target: loader
          anchors.horizontalCenterOffset: panel.offset
        }
      },
      State {
        when: layout.orientation === DockType.Vertical
        AnchorChanges {
          target: loader
          anchors.verticalCenter: panel.verticalCenter
        }
        PropertyChanges {
          target: loader
          anchors.verticalCenterOffset: panel.offset
        }
      }
    ]

    sourceComponent: layout.orientation === DockType.Horizontal ? panelHorizontal : panelVertical

    Component {
      id: panelVertical
      ColumnLayout {
        spacing: 0
      }
    }

    Component {
      id: panelHorizontal

      RowLayout {
        id: content
        spacing: 0

        Repeater {
          id: tasks

          model: tasksModel

          Icon {
            id: inner_icon
            dock: panel
            Layout.minimumWidth: width
            Layout.preferredWidth: width
            Layout.minimumHeight: height

            states: [
              State {
                when: layout.edge === DockType.Bottom
                PropertyChanges {
                  target: inner_icon
                  Layout.alignment: Qt.AlignBottom | Qt.AlignHCenter
                }
              },
              State {
                when: layout.edge === DockType.Top
                PropertyChanges {
                  target: inner_icon
                  Layout.alignment: Qt.AlignTop | Qt.AlignHCenter
                }
              }
            ]
          }
        }
      }
    }
  }
}
