import QtQuick 2.4
import QtQuick.Controls 2.3
import QtQuick.Layouts 1.3
import org.muelle.types 1.0

Pane {
    id: pane

    property alias alignment: alignment
    property alias edge: edge
    property alias behavior: behavior
    spacing: -1
    font.family: "Ubuntu"
    font.pointSize: 9

    contentWidth: height * 0.8
    contentHeight: columnLayout.height

    ColumnLayout {
        id: columnLayout
        anchors.right: parent.right
        anchors.left: parent.left
        anchors.top: parent.top

        GroupBox {
            id: alignment
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            title: qsTr("Alignment")
            background.visible: false

            property alias centerOffset: alignCenterOffset
            property var aligns: [alignStart, alignCenter, alignEnd]

            GridLayout {
                id: row0
                columnSpacing: 0
                columns: 3
                anchors.fill: parent
                anchors.horizontalCenter: parent.horizontalCenter

                Button {
                    id: alignStart
                    text: qsTr("Start")
                    hoverEnabled: true
                    autoExclusive: true
                    checkable: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: 0
                    property int value: Types.Start
                }

                Button {
                    id: alignCenter
                    text: qsTr("Center")
                    hoverEnabled: true
                    autoExclusive: true
                    checkable: true
                    Layout.fillWidth: true
                    Layout.preferredWidth: 0
                    property int value: Types.Center
                }

                Button {
                    id: alignEnd
                    text: qsTr("End")
                    hoverEnabled: true
                    autoExclusive: true
                    Layout.preferredWidth: 0
                    checkable: true
                    Layout.fillWidth: true
                    property int value: Types.End
                }

                Label {
                    id: label0
                    text: qsTr("Center offset")
                    Layout.columnSpan: 3
                    enabled: alignCenter.checked
                }

                Slider {
                    id: alignCenterOffset
                    snapMode: Slider.SnapOnRelease
                    stepSize: 0.01
                    hoverEnabled: true
                    wheelEnabled: true
                    orientation: Qt.Horizontal
                    from: -1
                    to: 1
                    enabled: alignCenter.checked
                    Layout.columnSpan: 3
                    Layout.fillWidth: true
                    live: true

                    ToolTip {
                        parent: alignCenterOffset.handle
                        visible: alignCenterOffset.hovered
                                 || alignCenterOffset.pressed
                        text: (alignCenterOffset.value * 100).toFixed(0) + '%'
                        font.pointSize: 9
                    }
                }
            }
        }

        GroupBox {
            id: edge
            title: qsTr("Edge")
            Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
            Layout.fillWidth: true
            background.visible: false

            property var edges: [edgeTop, edgeRight, edgeBottom, edgeLeft]

            GridLayout {
                id: row1
                columnSpacing: 0
                rowSpacing: 0
                columns: 2
                rows: 2
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.fill: parent

                Button {
                    id: edgeTop
                    text: qsTr("Top")
                    hoverEnabled: true
                    checkable: true
                    autoExclusive: true
                    Layout.preferredWidth: 0
                    Layout.fillWidth: true
                    property int value: Types.Top
                }

                Button {
                    id: edgeRight
                    text: qsTr("Right")
                    hoverEnabled: true
                    checkable: true
                    autoExclusive: true
                    Layout.preferredWidth: 0
                    Layout.fillWidth: true
                    property int value: Types.Right
                }

                Button {
                    id: edgeBottom
                    text: qsTr("Bottom")
                    hoverEnabled: true
                    checkable: true
                    autoExclusive: true
                    Layout.preferredWidth: 0
                    Layout.fillWidth: true
                    property int value: Types.Bottom
                }

                Button {
                    id: edgeLeft
                    text: qsTr("Left")
                    hoverEnabled: true
                    checkable: true
                    autoExclusive: true
                    Layout.preferredWidth: 1
                    Layout.fillWidth: true
                    property int value: Types.Left
                }
            }
        }

        GroupBox {
            id: behavior
            Layout.fillWidth: true
            title: qsTr("Behavior")
            background.visible: false

            property var behaviors: [behaviorDodgeAll, behaviorAutoHide, behaviorDodgeActive, behaviorAlwaysVisible]

            GridLayout {
                id: row2
                rowSpacing: 0
                columnSpacing: 0
                rows: 2
                columns: 2
                anchors.fill: parent

                Button {
                    id: behaviorDodgeAll
                    text: qsTr("Dodge windows")
                    hoverEnabled: true
                    checkable: true
                    autoExclusive: true
                    Layout.preferredWidth: 0
                    Layout.fillWidth: true
                    property int value: Types.DodgeAll
                }

                Button {
                    id: behaviorAutoHide
                    text: qsTr("Auto hide")
                    hoverEnabled: true
                    autoExclusive: true
                    checkable: true
                    Layout.preferredWidth: 0
                    Layout.fillWidth: true
                    property int value: Types.AutoHide
                }

                Button {
                    id: behaviorDodgeActive
                    text: qsTr("Dodge active window")
                    hoverEnabled: true
                    autoExclusive: true
                    checkable: true
                    Layout.preferredWidth: 0
                    Layout.fillWidth: true
                    property int value: Types.DodgeActive
                }

                Button {
                    id: behaviorAlwaysVisible
                    text: qsTr("Always visible")
                    hoverEnabled: true
                    checkable: true
                    autoExclusive: true
                    Layout.preferredWidth: 0
                    Layout.fillWidth: true
                    property int value: Types.AlwaysVisible
                }
            }
        }
    }
}

/*##^## Designer {
    D{i:0;autoSize:true;height:400;width:400}
}
 ##^##*/
