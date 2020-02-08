import QtQuick 2.12
import QtQml 2.3
import QtQuick.Controls 2.4
import QtQuick.Window 2.11
import '../../shared/components'
import '../../shared/functional.ts' as F

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
    initial: false
    Bind {
      target: win
      property: 'visible'
    }
    Bind {
      target: menu
      property: 'visible'
    }
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
          menuItem.name = win.model[index].name
          menuItem.trigger = win.model[index].trigger
        }
      }

      onObjectAdded: menu.insertItem(index, object)
      onObjectRemoved: menu.removeItem(object)
    }
  }
}

