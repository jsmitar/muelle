import QtQuick 2.14

import '../Store/actions.ts' as Action
import '../../shared/functional.ts' as F

DropArea {
  id: dragAndDrop

  property Item target

  property Item dragging
  property Item hovered

  property string removedLauncher: ''

  readonly property rect rect: store.state.geometry.panelNextRect

  anchors {
    fill: target
    topMargin: rect.y
    rightMargin: target.width - rect.width - rect.x
    bottomMargin: target.height - rect.height - rect.y
    leftMargin: rect.x
  }

  function moveTask(dragging, hovered) {
    if (dragging && hovered) {
      const { move } = store.tasksModel
      move(dragging.index, hovered.index)
    }
  }

  function syncLaunchers() {
    store.dispatch(Action.syncLaunchers())
  }

  function findLauncher(url) {
    const url_ = `${url}`.split('/').reverse()[0]

    return F.find(
      target.positioner.visibleChildren, 
      item => { 
        return item.objectName === 'IconTaskContainer' && 
              `${item.m.LauncherUrlWithoutIcon}`.endsWith(url_)
      }
    ) || null
  }

  property var positionHandler: F.throttle(() => {
    const prevAppId = hovered && hovered.m ? hovered.m.AppId : ''

    hovered = target.positioner.childAt(drag.x, drag.y)
    const appId = hovered && hovered.m ? hovered.m.AppId : ''

    if (prevAppId !== appId) {
      moveTask(dragging, hovered)
    }
  }, 250)

  function addLauncher(url) {
    store.backend.addLauncher(url)
  }

  function isApplication(url) {
    return url && store.backend.isApplication(url)
  }

  drag.onPositionChanged: positionHandler()

  property var removeLauncher: F.debounce(() => {
    if ($view.containsMouse) return

    if (dragging && dragging.m.IsLauncher) {
      const url = dragging.m.LauncherUrlWithoutIcon
      store.tasksModel.requestRemoveLauncher(url)
      removedLauncher = url
      dragging = null
    }
  }, 400)

  Connections {
    target: $view

    onExited: removeLauncher()
  }

  onExited: removeLauncher()

  onEntered: {
    const url = drag.hasUrls ? drag.urls[0] : removedLauncher

    if (removedLauncher || isApplication(url)) {
      addLauncher(url)
      dragging = findLauncher(url)

      drag.accept()
    }
    removedLauncher = ''
  }

  onDropped: {
    if (dragging && dragging.objectName === 'IconTaskContainer') {
      syncLaunchers()
    } else {
      const url = drop.hasUrls ? drop.urls[0] : ''

      if (url) {
        addLauncher(url)
        syncLaunchers()
      } else if (hovered) {
        const modelIndex = store.tasksModel.makeModelIndex(hovered.index)
        F.each(drop.urls, url => {
          store.tasksModel.requestOpenUrls(modelIndex, drop.urls)
        })
      }
    }

    dragging = null
    hovered = null
    removedLauncher = ''
  }
} 