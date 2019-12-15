import QtQuick 2.12
import org.muelle.types 1.0

StateGroup {
  property alias reset: _reset.changes
  property alias top: _top.changes
  property alias right: _right.changes
  property alias bottom: _bottom.changes
  property alias left: _left.changes

  states: [
    State {
      id: _reset
      name: 'reset'
    },
    State {
      id: _top
      extend: 'reset'
      name: 'top'
      when: $layout.edge & Types.Top
    },
    State {
      id: _right
      extend: 'reset'
      name: 'right'
      when: $layout.edge & Types.Right
    },
    State {
      id: _bottom
      extend: 'reset'
      name: 'bottom'
      when: $layout.edge & Types.Bottom
    },
    State {
      id: _left
      extend: 'reset'
      name: 'left'
      when: $layout.edge & Types.Left
    }
  ]
}
