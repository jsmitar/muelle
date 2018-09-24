import QtQuick 2.11
import QtQuick.Layouts 1.1
import org.duckdock.types 1.0
import "qrc:/utils.js" as F

Item {
  id: root
  width: iconSize * 10 + iconZoom + iconZoomAmplitude * 2
  height: iconZoom
  clip: false
  visible: true

  property int iconSize: 80
  property real iconZoomFactor: 2
  property int iconZoom: iconSize * iconZoomFactor
  property int iconZoomAmplitude: (iconZoom - iconSize) / 2

  property var zoom: F.zoom(iconSize, iconZoom, iconZoomAmplitude)

  property var tasksModel: F.gen(11, function (i) {
    return {
      id: i,
      index: i
    }
  })

  Component.onCompleted: {
    layout.edge = DockType.Top
    console.log(layout.edge, layout.orientation, layout.alignment,
                layout.layout, view)
  }

  Connections {
    target: layout

    onLayoutChanged: {
      console.log(layout.layout)
    }
  }

  DockArea {
    id: dockArea
    width: root.width
    height: root.height
    antialiasing: true
  }
}
