import QtQuick 2.14

Loader {
  property bool when: false

  property Component then
  property Component otherwise

  sourceComponent: (when ? then : otherwise) || undefined
}