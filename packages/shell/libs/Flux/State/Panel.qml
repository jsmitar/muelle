import QtQuick 2.10
import org.muelle.types 1.0
import "./" as Record
import "../../../Extras"

QObject {
  id: panel

  property bool lockVisible: false
  property bool visible: false
  readonly property bool hidden: !visible

  property string slide: 'None'

  property bool updatingPosition: false
  property int nextEdge: Types.Top
  property int nextAlignment: Types.Start

  readonly property int orientation: edge & Types.Top || edge & Types.Bottom
      ? Types.Horizontal : Types.Vertical

  readonly property bool isHorizontal: orientation === Types.Horizontal
  readonly property bool isVertical: orientation === Types.Vertical
  property int alignment: Types.Start
  property int edge: Types.Top
  property int behavior: Types.None

  // counter of not grouped tasks
  property int nextTaskCount1: 0
  property int taskCount1: 0
}
