import QtQuick 2.14
import org.muelle.extra 1.0 as Muelle

Flow {
  id: indicator

  flow: $layout.isHorizontal ? Flow.LeftToRight : Flow.TopToBottom
  spacing: 2
  padding: 2

  property bool isActive: false
  property bool isGroupParent: false
  property bool isLauncher: false
  property bool isDemandingAttention: false

  visible: !isLauncher

  readonly property color colorNormal: '#828282'
  readonly property color colorAttention: '#F2994A'
  readonly property color colorActive: '#2D9CDB'

  readonly property color color: 
    isDemandingAttention 
      ? colorAttention
      : isActive
        ? colorActive
        : colorNormal

  Pointer {
    color: indicator.color
    glow: isActive || isDemandingAttention
  }
  Pointer {
    visible: isGroupParent
    color: indicator.color
    glow: isActive || isDemandingAttention
  }

  StateLayoutEdge {
    reset: AnchorChanges {
      target: indicator
      anchors {
        top: undefined
        right: undefined
        bottom: undefined
        left: undefined
        verticalCenter: undefined
        horizontalCenter: undefined
      }
    }
    top: AnchorChanges {
      target: indicator
      anchors {
        top: parent.top
        horizontalCenter: parent.horizontalCenter
      }
    }
    right: AnchorChanges {
      target: indicator
      anchors {
        right: parent.right
        verticalCenter: parent.verticalCenter
      }
    }
    bottom: AnchorChanges {
      target: indicator
      anchors {
        bottom: parent.bottom
        horizontalCenter: parent.horizontalCenter
      }
    }
    left: AnchorChanges {
      target: indicator
      anchors {
        left: parent.left
        verticalCenter: parent.verticalCenter
      }
    }
  }
}