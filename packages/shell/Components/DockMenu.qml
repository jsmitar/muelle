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

      win.x = F.clamp(0, x, Math.max(x, $view.screen.geometry.right) - menu.width)
      win.y = F.clamp(0, y, Math.max(y, $view.screen.geometry.bottom) - menu.height)
    }
  }

  width: menu.width
  height: menu.height

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

        text: win.model[index].text
        onTriggered: win.model[index].trigger()
      }

      onObjectAdded: menu.insertItem(index, object)
      onObjectRemoved: menu.removeItem(object)
    }
  }
}

