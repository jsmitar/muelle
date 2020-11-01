import QtQuick 2.14
import QtQuick.Window 2.12
import org.muelle.types 1.0
import 'qrc:/shared/components'
import 'qrc:/shared/functional.ts' as F

QObject {
  objectName: '@State/Geometry'

  property var state

  property bool composition: true

  readonly property bool isHorizontal: state.panel.isHorizontal

  readonly property bool isTopOrLeft:
    (state.panel.edge & (Types.Top | Types.Left)) > 0 

  /*
   * The geometry creation aproach is calculate 
   * sizes(width,height), position(x,y) as Horizontal Panel 
   * and (Top,Left) or (Bottom,Right) edge panel
   * The last step is turn the values sizes, point and rects for create 
   * the geometries for each Panel Edge
   *
   * ____________________________________________
   * |            |          |  | shadow        |
   * |            |----------|--|---------------|
   * |            |          |  | paddingTop    |
   * |            |          |--|---------------|
   * |            | panel    |  | icon.size +   |
   * | background |          |  | spacing       |
   * |            | inputMask|--|---------------|
   * |            |          |  | paddingBottom |
   * |            |----------|--|---------------|
   * |            |          |  | inset         |
   * |____________|__________|__|_______________|
   */


  readonly property QObject view: QObject {
    id: view
    objectName: 'view'

    readonly property int width: 
      isHorizontal ? Screen.width : Screen.height

    readonly property int height:
      Math.max(
        background.boxHeight, 
        panel.height + background.inset
      )

    readonly property size size: isHorizontal
      ? Qt.size(width, height)
      : Qt.size(height, width)
  } // view

  readonly property QObject background: QObject {
    id: background
    objectName: 'background'

    readonly property int shadow: state.background.shadowBlur

    readonly property int paddingX: state.icon.size * state.background.paddingX

    readonly property int paddingTop: state.icon.size * state.background.paddingTop

    readonly property int paddingBottom: state.icon.size * state.background.paddingBottom

    readonly property int inset: Math.max(0, state.background.inset)

    readonly property int extents: 
      Math.max(0, view.height - Math.max(panel.height, height) - inset)

    readonly property bool isAtStart: x <= 0

    readonly property bool isAtEnd: x + width >= view.width

    readonly property int x: F.clamp(0, panel.x - background.paddingX, view.width - width)

    readonly property int y: isTopOrLeft
      ? state.background.inset
      : view.height - height - inset

    readonly property int width: state.background.fill
      ? view.width 
      : Math.min(view.width, paddingX * 2 + panel.width)

    readonly property int height: 
      paddingTop + state.icon.size + paddingBottom

    readonly property int boxHeight:
      shadow + paddingTop + state.icon.size + paddingBottom + inset

    readonly property point point: isHorizontal 
      ? Qt.point(x, y)
      : Qt.point(y, x)

    readonly property size size: isHorizontal
      ? Qt.size(width, height)
      : Qt.size(height, width)

    readonly property rect rect: isHorizontal
      ? Qt.rect(x, y, width, height)
      : Qt.rect(y, x, height, width)

    readonly property QObject next: QObject {
      objectName: 'next'
      readonly property int x: F.clamp(0, panel.next.x - background.paddingX, view.width - width)

      readonly property int y: background.y

      readonly property int width: state.background.fill
        ? view.width
        : Math.min(view.width, background.paddingX * 2 + panel.next.width)

      readonly property int height: background.height

      readonly property point point: isHorizontal 
        ? Qt.point(x, y)
        : Qt.point(y, x)

      readonly property size size: isHorizontal
        ? Qt.size(width, height)
        : Qt.size(height, width)

      readonly property rect rect: isHorizontal
        ? Qt.rect(x, y, width, height)
        : Qt.rect(y, x, height, width)
    } // background.next
  } // background

  readonly property QObject panel: QObject {
    id: panel
    objectName: 'panel'

    property real offset: 0

    readonly property int xCenter: (view.width - width) / 2

    readonly property int x_: xCenter + offset * (xCenter - background.paddingX + 1)

    readonly property int x: F.clamp(background.paddingX, x_, view.width - width - background.paddingX) 

    readonly property int y: isTopOrLeft 
      ? background.paddingBottom + background.inset
      : view.height - height - background.paddingBottom - background.inset

    readonly property int width:
      state.icon.size * state.panel.taskCount1 +
      state.icon.separator * state.panel.separatorCount +
      state.icon.spacing * Math.max(0, state.panel.taskCount1 + state.panel.separatorCount - 1)

    readonly property int height: 
      state.icon.size +
      background.paddingBottom + Math.max(0, background.paddingTop)

    readonly property size size: isHorizontal
      ? Qt.size(width, height)
      : Qt.size(height, width)

    readonly property point point: isHorizontal
      ? Qt.point(x, y)
      : Qt.point(y, x) 

    readonly property rect rect: isHorizontal
      ? Qt.rect(x, y, width, height)
      : Qt.rect(y, x, height, width)

    readonly property QObject next: QObject {
      id: next
      objectName: 'next'

      readonly property int xCenter: (view.width - width) / 2

      readonly property int x_: xCenter + panel.offset * (xCenter - background.paddingX + 1)

      readonly property int x: F.clamp(background.paddingX, x_, view.width - width - background.paddingX) 

      readonly property int y: panel.y

      readonly property int width:
        state.icon.size * state.panel.nextTaskCount1 +
        state.icon.separator * state.panel.separatorCount +
        state.icon.spacing * Math.max(0, state.panel.nextTaskCount1 + state.panel.separatorCount - 1)

      readonly property int height: panel.height

      readonly property size size: isHorizontal
        ? Qt.size(width, height)
        : Qt.size(height, width)

      readonly property point point: isHorizontal
        ? Qt.point(x, y)
        : Qt.point(y, x) 
      
      readonly property rect rect: isHorizontal
        ? Qt.rect(x, y, width, height)
        : Qt.rect(y, x, height, width)
    } // panel.next
  } // panel

  readonly property QObject mask: QObject {
    objectName: 'mask'
    property bool growing: false

    property bool enabled: true

    property bool fill: 
      state.panel.updatingOffset ||
      state.panel.updatingOrientation

    readonly property var region: 
      fill 
        ? mask.visible.regionFill 
        : enabled 
          ? state.panel.visible 
            ? mask.visible.region
            : mask.hidden.region
          : mask.visible.regionFill

    readonly property QObject visible: QObject {
      objectName: 'visible'
      readonly property var region: mask.growing
        ? Qt.region(background.next.rect, panel.next.rect)
        : Qt.region(background.rect, panel.rect)

      readonly property var regionFill: Qt.region()
    } // mask.visible

    readonly property QObject hidden: QObject {
      objectName: 'hidden'
      readonly property var region: Qt.region(Qt.rect(-1, -1, 1, 1))
    } // mask.hidden
  } // mask
} // @State/Geometry
