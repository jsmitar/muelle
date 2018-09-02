import QtQuick 2.11
import QtQuick.Layouts 1.1

MouseArea {
  id: icon

  property Item dock
  property int index: modelData.index
  property int taskId: modelData.id

  property real factorSize: 1.0
  property int _size: iconSize

  hoverEnabled: true

  width: _size
  height: _size

  Connections {
    target: dock

    onDisableInitialAnimations: {
      initialAnimation.enabled = false
    }
  }

  Behavior on _size {
    id: initialAnimation
    SequentialAnimation {
      NumberAnimation {
        duration: 40
      }
    }
  }

  Connections {
    target: icon

    enabled: icon.containsMouse

    onEntered: dock.taskHovered = index

    onMouseXChanged: {

      _each(_filter(dock.tasksItems, function (task) {
        return Math.abs(task.index - icon.index) > 1
      }), function (task) {
        if (task._size)
          task._size = iconSize
      })
      _size = iconZoom

      var siblings = dock.siblings(icon)
      if (!siblings.length)
        return

      var absolute = mouseX
      var calcSize = function (pos, min, max) {
        return Math.min(Math.max(pos / 4 + min, min), max) | 0
      }

      var size = calcSize(absolute, iconSize, iconZoom)

      siblings[0]._size = Math.max(iconZoom - size / 2, iconSize)
      if (siblings.length > 1) {
        siblings[1]._size = size
      }
    }
  }

  Rectangle {
    property real _sizeProxy: icon._size - 4

    color: randColor()
    radius: iconSize
    anchors {
      horizontalCenter: parent.horizontalCenter
      bottom: parent.bottom
      alignWhenCentered: false
    }

    width: _sizeProxy
    height: _sizeProxy

    Text {
      //text: String.fromCharCode(Math.random() * 25 + 66)
      text: icon._size | 0
      anchors.centerIn: parent
      style: Text.Raised
      color: randColor()
    }
  }
  //  Image {
  //    source: 'file://usr/share/icons/hicolor/64x64/apps/okular.png'
  //    fillMode: Image.PreserveAspectFit
  //    cache: false
  //    width: _size
  //    height: _size
  //    mipmap: false
  //    smooth: false
  //    sourceSize.width: _size
  //    sourceSize.height: _size
  //    anchors {
  //      horizontalCenter: parent.horizontalCenter
  //      bottom: parent.bottom
  //      alignWhenCentered: false
  //    }
  //  }
}
