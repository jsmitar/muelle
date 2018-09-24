import QtQuick 2.11
import QtQuick.Layouts 1.1
import org.duckdock.types 1.0
import "qrc:/utils.js" as F

MouseArea {
  id: icon
  objectName: 'icon'

  property Item dock
  property int index: modelData.index
  property int taskId: modelData.id
  property int _size: iconSize
  property Behavior animation: animation
  property int orientation: layout.orientation
  property real pos: 0
  // [..., l0, center, r0, ...]
  property var siblings: ({
                            l0: null,
                            c: null,
                            r0: null
                          })

  signal updated

  hoverEnabled: true

  width: _size
  height: _size

  StateGroup {
    states: [
      State {
        when: layout.orientation === DockType.Horizontal
        PropertyChanges {
          target: icon
          pos: mouseX
        }
      },
      State {
        when: layout.orientation === DockType.Vertical
        PropertyChanges {
          target: icon
          pos: mouseY
        }
      }
    ]
  }

  Behavior on _size {
    id: animation
    enabled: false
    SequentialAnimation {
      SmoothedAnimation {
        id: anim
        alwaysRunToEnd: true
        duration: 100
      }
    }
  }

  Connections {
    target: view

    onEntered: {
      animation.enabled = true
    }
  }

  onEntered: {
    dock.taskHovered = index
    start()
  }

  onExited: {
    posChanged.disconnect(updated)
    if (_size != iconZoom)
      return
    F.each(dock.taskItems, function (task) {
      task.animation.enabled = false
    })
  }

  Timer {
    id: timer
    interval: 10
    running: false
  }

  function start() {
    _size = iconZoom
    siblings = dock.siblings(icon)

    F.each(F.filter(dock.taskItems, function (t) {
      return Math.abs(index - t.index) > 1
    }), function (t) {
      t._size = iconSize
    })

    F.timeout(timer, function () {
      if (containsMouse) {
        posChanged.connect(updated)
        updated()
      }
    }, 10)
  }

  onUpdated: {
    var relative = -iconZoom / 2 + F.boundary(0, pos, iconZoom)
    var z = 0

    if (siblings.l0) {
      z = Math.ceil(zoom(-relative))
      siblings.l0._size = z
    } else {
      z = iconZoom - zoom(relative)
      dock.offset = Math.ceil(z / 2)
    }

    if (siblings.r0) {
      z = Math.floor(zoom(relative))
      siblings.r0._size = z
    } else {
      z = iconZoom - zoom(-relative)
      dock.offset = -Math.floor(z / 2)
    }
  }

  Rectangle {
    property real _sizeProxy: icon._size

    color: F.randColor(.1)
    border {
      color: F.randColor()
      width: 3
    }
    radius: width
    anchors {
      horizontalCenter: parent.horizontalCenter
      top: parent.top
      alignWhenCentered: false
    }

    width: _sizeProxy
    height: _sizeProxy

    Text {
      //text: String.fromCharCode(Math.random() * 25 + 66)
      smooth: true
      text: icon._size
      font.family: "Source Code Pro Black"
      font.bold: true
      verticalAlignment: Text.AlignBottom
      horizontalAlignment: Text.AlignHCenter
      font.pixelSize: 16
      fontSizeMode: Text.FixedSize
      anchors.centerIn: parent
      style: Text.Raised
      styleColor: "#e6e6e6"
      color: parent.border.color
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
