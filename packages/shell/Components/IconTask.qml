import QtQuick 2.12
import QtQuick.Layouts 1.1
import org.muelle.types 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import '../helpers/constants.ts' as Const
import '../../shared/functional.ts' as F
import '../Containers'
import '../../shared/components'
import QtGraphicalEffects 1.12

IconTaskContainer {
  id: icon
  objectName: 'IconTask'
  iconTarget: innerIcon

  Layout.minimumWidth: innerIcon.width
  Layout.minimumHeight: innerIcon.height

  DropShadow {
    source: innerIcon
    anchors.fill: innerIcon
    horizontalOffset: 0
    verticalOffset: 0
    radius: 4
    samples: 1 + radius * 2
    color: "#80000000" 
  }

  Box {
    id: innerIcon
    objectName: 'innerIcon'
    width: icon.size
    height: icon.size
    paddings: icon.padding

    PlasmaCore.IconItem {
      id: iconItem
      source: m.decoration
      roundToIconSize: true
      usesPlasmaTheme: false
    }

    resources: [
      PaintItem { target: innerIcon; enabled: false; showSize: true },
      PaintItem { target: iconItem; enabled: false; showSize: false }
    ]
  }
}
