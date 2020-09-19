import QtQuick 2.14
import QtQuick.Layouts 1.14
import QtGraphicalEffects 1.14

import org.kde.plasma.core 2.0 as PlasmaCore
import org.muelle.extra 1.0 as Muelle
import org.muelle.types 1.0

import 'qrc:/shared/functional.ts' as F
import 'qrc:/shared/components'

Component {
  PlasmaCore.Dialog {
    id: taskGroup

    outputOnly: true
    backgroundHints: PlasmaCore.Dialog.NoBackground
    type: PlasmaCore.Dialog.PopupMenu
    flags: Qt.WindowStaysOnTopHint | Qt.ToolTip
    location: PlasmaCore.Types.Floating
    hideOnWindowDeactivate: true

    Component.onCompleted: {
      let timer = 0

      F.on(hover_Changed, labelChanged, _ => {
        Qt.clearTimeout(timer)
        visible = false
        timer = Qt.setTimeout(_ => (visible = hover_), 50)
      })
       .clear(destruction)
    }

    signal destruction
    Component.onDestruction: destruction()

    property var m
    property bool hover: false
    property string label: F.get(m, 'display', '') || F.get(m, 'AppName', '')
    property var winIdList: F.get(m, 'WinIdList', [])
    property bool isMinimized: F.get(m, 'IsMinimized', true)
    property bool hover_: label && (
      hover || 
      $view.containsMouse ||
      box.mouse.containsMouse
    )

    property var _: Spy {
      properties: [spy`${taskGroup}.hover_`]
    }
    
    readonly property var edgeToLocation: ({
      [Types.Top]: PlasmaCore.Types.TopEdge,
      [Types.Right]: PlasmaCore.Types.RightEdge,
      [Types.Bottom]: PlasmaCore.Types.BottomEdge,
      [Types.Left]: PlasmaCore.Types.LeftEdge
    })

    mainItem: Item {
      id: box

      width: content.width + 10 * 2
      height: content.height + 10 * 2

      Muelle.Rectangle {
        id: bg

        anchors.centerIn: parent
        width: content.width
        height: content.height
        
        radius {
          topLeft: 10
          topRight: 10
          bottomLeft: 10
          bottomRight: 10
        }
        property var edgeToDegrees: ({
          [Types.Top]: 0,
          [Types.Right]: 270,
          [Types.Bottom]: 180,
          [Types.Left]: 90,
        })

        border {
          width: 1
          gradient {
            stops: ['#111 1', '#222 0']
            degrees: edgeToDegrees[store.state.panel.edge]
          }
        }
        gradient {
          stops: ['#222 1', '#111 0']
          degrees: F.addDeg(edgeToDegrees[store.state.panel.edge], 0)
        }

        layer.enabled: true
        layer.effect: Glow {
          id: shadow

          readonly property int shadowBlur: store.state.background.shadowBlur

          anchors.fill: bg
          source: bg

          color: bg.color
          cached: false
          spread: 0.1
          samples: 2 * radius + 1
          radius: shadowBlur / 2
          transparentBorder: true
        }

        ColumnLayout {
          id: content
          anchors.centerIn: parent
          spacing: 0

          Text {
            id: label_
            Layout.alignment: Qt.AlignLeft
            Layout.maximumWidth: taskGroup.winIdList.length 
              ? taskGroup.winIdList.length * (300 - (leftPadding + rightPadding))
              : -1

            topPadding: 4
            leftPadding: 8
            bottomPadding: 4
            rightPadding: 8

            text: label
            color: 'white'
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
            maximumLineCount: 1
          }

          RowLayout {
            id: thumbnails
            visible: !!taskGroup.winIdList
            Layout.leftMargin: 2
            Layout.rightMargin: 2
            Layout.bottomMargin: 2
            spacing: 2

            Repeater {
              model: winIdList

              Item {
                Layout.preferredWidth: mask.width
                Layout.preferredHeight: mask.height
                Layout.alignment: Qt.AlignVCenter
                Layout.fillHeight: false

                visible: !isMinimized
                layer.enabled: visible
                layer.effect: OpacityMask {
                  id: opacityMask
                  maskSource: mask
                }

                Muelle.Rectangle {
                  id: mask
                  radius {
                    topLeft: 10
                    topRight: 10
                    bottomLeft: 10
                    bottomRight: 10
                  }
                  visible: true
                  width: thumbnail.paintedWidth
                  height: thumbnail.paintedHeight
                }
                PlasmaCore.WindowThumbnail {
                  id: thumbnail
                  winId: modelData
                  width: 300
                  height: 250
                  anchors.centerIn: parent
                }
              }
            }
          }
        }
      }

      property MouseArea mouse: MouseArea {
        id: mouse
        parent: box
        anchors.fill: parent
        hoverEnabled: true
        onEntered: {
          console.log('ENTERED')
        }
      }
    }
  }
}