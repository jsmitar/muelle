import QtQuick 2.11
import QtQml 2.3
import QtQuick.Controls 2.4
import QtQuick.Window 2.11
import "../Extras"
import '../libs/functional.ts' as F

Window {
  id: win
  flags: Qt.Popup | Qt.FramelessWindowHint

  property var model: []
  property Item parent

  onVisibleChanged: {
    if (visible) {
      const {x, y} = $view.mousePosition
      win.x = Math.min(x, win.screen.width - menu.width)
      win.y = Math.min(y, win.screen.height - menu.height)
    }
  }

  width: menu.width
  height: 30

  BindingModel {
    source: win
    target: menu
    property: 'visible'
  }

  Menu {
    id: menu

    Instantiator {
      model: win.model.length
      delegate: MenuItem {
        id: menuItem
        property var name: ''
        property var trigger: F.noop

        onTriggered: trigger(name)
        Component.onCompleted: {
          F.assign(menuItem, win.model[index])
        }
      }

      onObjectAdded: menu.insertItem(index, object)
      onObjectRemoved: menu.removeItem(object)
    }
  }
}

