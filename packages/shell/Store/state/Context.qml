import QtQuick 2.0

QtObject {
  objectName: "@State/Context"

  readonly property QtObject view: $view

  readonly property QtObject layout: $layout

  readonly property QtObject positioner: $positioner

  readonly property QtObject configuration: $configuration
}
