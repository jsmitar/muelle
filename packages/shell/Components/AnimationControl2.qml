import QtQuick 2.10
import '../../shared/functional.ts' as F

QtObject {
  id: control
  property Animation target: parent
  property var _finishCallback: undefined
}
