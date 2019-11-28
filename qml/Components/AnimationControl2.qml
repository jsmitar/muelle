import QtQuick 2.10
import "../libs/functional.js" as F

QtObject {
  id: control
  property Animation target: parent
  property var _finishCallback: undefined
}
