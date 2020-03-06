import QtQuick 2.12
import QtQuick.Layouts 1.1
import QtQml.Models 2.3
import org.muelle.types 1.0
import org.muelle.extra 1.0
import org.kde.taskmanager 0.1 as TaskManager
import '../../shared/functional.ts' as F
import '../Components'
import '../../shared/components'

Item {
  id: dockContainer

  PanelSlideAnimation {
    target: panelContainer
  }

  ViewShadows {
    view: $view
    geometry: store.state.geometry.panelNextRect
    enabled: store.state.panel.visible
    edges: 
      (Types.Top | Types.Right | Types.Bottom | Types.Left) 
      & ~(store.state.panel.edge)
  }

  PanelContainer {
    id: panelContainer
    anchors.centerIn: dockContainer
    width: dockContainer.width
    height: dockContainer.height

    DelegateModel {
      Component.onCompleted: {
        Qt.callLater(() => {
          model = Qt.binding(() => store.tasksModel)
        })
      }

      Component.onDestruction: {
        model = null
      }

      delegate: IconTask {
        id: icon
        panel: panelContainer
      }
    }
  }
}
