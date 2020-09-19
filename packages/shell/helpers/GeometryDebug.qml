import QtQuick 2.14
import QtQuick.Layouts 1.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14

import 'qrc:/shared/functional.ts' as F
import 'qrc:/shared/components'

ApplicationWindow {
  id: win

  property var geometry: store.state.geometry

  width: 1024
  minimumHeight: frame.height + scroll.height
  maximumHeight: frame.height + scroll.height

  function filterKeys(target) {
    return Object.keys(target)
      .filter(key => {
        return !F.isCallable(target[key]) 
            && !['objectName', 'state', 'resources', 'parentObject'].includes(key)
      })
      .map(key => ({ target, key }))
  }

  Component {
    id: groupComponent

    GroupBox {
      id: group
      property var target: ({})
      
      property int column: 0
      width: Math.max(label.width, flow.width) + 10
      
      label: Label {
        text: F.get(target, 'objectName') || group.objectName
        font.pixelSize: 18
        font.weight: Font.Bold
        leftPadding: 5
        renderType: Text.NativeRendering
      }
      Flow {
        id: flow
        flow: Flow.TopToBottom
        height: F.filter(children, child => !F.get(child, 'isQtObject', true))
                  .reduce((height, child) => height + F.get(child, 'height', 0), 0)

        Repeater {
          model: filterKeys(target)
          
          ConditionalLoader {
            id: loader
            property var prop: target[modelData.key]

            property bool isQtObject: F.isQtObject(prop)

            when: !isQtObject

            onLoaded: {
              if ('target' in item) {
                item.target = Qt.binding(() => prop)
                item.objectName = Qt.binding(() => modelData.key)
              }
            }

            then: Component {
              Row {
                spacing: 5
                rightPadding: 10
                Label {
                  text: `${modelData.key}:` 
                  font.weight: Font.Bold
                  renderType: Text.NativeRendering
                }
                Label {
                  id: textValue
                  text: F.tostr(prop, 2, 2)
                  renderType: Text.NativeRendering

                  Behavior on text {
                    SequentialAnimation {
                      ColorAnimation {
                        target: textValue; property: 'color'
                        to: '#ffffff'; duration: 0
                      }
                      ColorAnimation {
                        target: textValue; property: 'color'
                        to: "#FF6771"; duration: 250
                      }
                      PauseAnimation{
                        duration: 1000
                      }
                      ColorAnimation {
                        target: textValue; property: 'color'
                        to: '#00bbd2'; duration: 100
                      }
                    }
                  }
                }

              } // Row
            } // Component
            otherwise: groupComponent
          } // ConditionalLoader
        } // Repeater
      } // ColumnLayout
    } // GroupBox
  }

  Flickable {
    width: win.width
    height: win.height
    contentWidth: frame.width
    contentHeight: frame.height

    ScrollBar.horizontal: ScrollBar { id: scroll }

    Frame {
      id: frame

      Flow {
        id: flow
        spacing: -1
        height: children[0].height
        flow: Flow.TopToBottom
        Repeater {
          model: ['geometry', 'panel', 'icon', 'animation', 'background']

          ConditionalLoader {
            when: true
            then: groupComponent

            onLoaded: {
              if ('target' in item) {
                item.target = Qt.binding(() => store.state[modelData])
              }
            }
          }
        }
      }
    }
  }
}