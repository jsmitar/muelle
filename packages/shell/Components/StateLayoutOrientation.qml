import QtQuick 2.12

StateGroup {
  property alias reset: _reset.changes
  property alias horizontal: _horizontal.changes
  property alias vertical: _vertical.changes

  states: [
    State {
      id: _reset
      name: 'reset'
    },
    State {
      id: _horizontal
      extend: 'reset'
      name: 'horizontal'
      when: $layout.isHorizontal
    },
    State {
      id: _vertical
      extend: 'reset'
      name: 'vertical'
      when: $layout.isVertical
    }
  ]
}
