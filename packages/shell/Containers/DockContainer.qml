import QtQuick 2.11
import QtQuick.Layouts 1.1
import QtQml.Models 2.3
import org.muelle.types 1.0
import org.kde.taskmanager 0.1 as TaskManager
import '../libs/functional.ts' as F
import "../Components"
import "../Extras"

Item {
  id: dockContainer

  Binding {
    target: slide
    property: 'target'
    value: panelContainer
  }

  PanelContainer {
    id: panelContainer
    anchors.centerIn: dockContainer
    width: dockContainer.width
    height: dockContainer.height

    DelegateModel {
      Component.onCompleted: {
        setTimeout(_ => {
          model = Qt.binding(_ => store.tasksModel)
        }, 500)
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
