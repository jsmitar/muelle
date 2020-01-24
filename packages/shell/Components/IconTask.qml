import QtQuick 2.12
import QtQuick.Layouts 1.1
import org.muelle.types 1.0
import org.kde.plasma.core 2.0 as PlasmaCore
import '../helpers/constants.ts' as Const
import '../../shared/functional.ts' as F
import '../Containers'
import '../../shared/components'

IconTaskContainer {
  id: icon
  objectName: 'IconTask'
  iconTarget: innerIcon

  Layout.minimumWidth: innerIcon.width
  Layout.minimumHeight: innerIcon.height

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
      PaintItem { target: innerIcon; enabled: true; showSize: true },
      PaintItem { target: iconItem; enabled: true; showSize: false }
    ]
  }
}
