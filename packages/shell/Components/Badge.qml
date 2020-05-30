import QtQuick 2.14
import 'qrc:/shared/functional.ts' as F
import org.kde.plasma.core 2.0 as PlasmaCore

Rectangle {
  id: badge

  property alias label: _label.text

  readonly property int m: Math.max(
    theme.mSize().width, 
    theme.mSize().height
  ) - 2

  width: Math.max(m - m % 2, _label.contentWidth + 4)
  height: m - m % 2
  radius: height

  color: theme.highlightColor

  Text {
    id: _label
    
    anchors.centerIn: badge

    color: theme.highlightedTextColor
    horizontalAlignment: Text.AlignLeft
    verticalAlignment: Text.AlignVCenter

    font {
      weight: Font.Black
      bold: true
      pixelSize: 10
    }
  }
}