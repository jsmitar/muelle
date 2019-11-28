import QtQuick 2.0
import QtQuick.Window 2.3
import "../Extras"

Window {
  id: win

  title: qsTr('Settings: DuckDock')
  width: 380
  height: settings.height
  minimumWidth: 380
  maximumWidth: 600
  minimumHeight: settings.height
  maximumHeight: settings.height

  Loader {
    id: settings
    sourceComponent: settingsComponent
    active: win.visible
  }

  Component {
    id: settingsComponent

    Settings {
      id: settings
      width: win.width
    }
  }
}
