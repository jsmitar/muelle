import QtQuick 2.14
import QtQuick.Window 2.3
import 'qrc:/shared/components'
import 'qrc:/shell/Store/actions.ts' as Action

Window {
  id: win

  title: qsTr('Settings: Muelle')
  width: 380
  height: settings.height
  minimumWidth: 380
  maximumWidth: 600
  minimumHeight: settings.height
  maximumHeight: settings.height
  flags: Qt.WindowStaysOnTopHint

  BindingValue {
    target: store.state.settings
    property: 'visible'
    then: value => win.visible = value
    otherwise: then
  }

  onVisibleChanged: {
    store.dispatch(Action.showSettings(visible))
  }

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
