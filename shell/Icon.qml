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
        duration: 10
      }
    }
  }

  function calcZoom(mousePosition) {
    return Math.min(
          Math.max(
            iconSize + iconZoomAmplitude
            + (-mousePosition * (iconZoom - iconSize - iconZoomAmplitude)) / (iconZoom / 2),
            iconSize), iconZoom)
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

      var relative = -iconZoom / 2 + mouseX | 0

      var l = Math.ceil(calcZoom(relative))
      var r = Math.floor(calcZoom(-relative))

      //l = Math.min(iconSize + iconZoomAmplitude, l)
      //r = Math.min(iconSize + iconZoomAmplitude, r)
      siblings[0]._size = l

      if (siblings.length > 1)
        siblings[1]._size = r

      console.log('l:' + siblings[0]._size, 'r:' + siblings[1]._size,
                  siblings[0]._size + siblings[1]._size)
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
