import QtQuick 2.10
import org.muelle.types 1.0
import './types.ts' as T

QtObject {
  property FluxStore store
  readonly property FluxState state: store.state

  readonly property var mutations: ({
    [T.SLIDE_STATUS](status) {
      state.panel.slide = status
    },
    [T.UPDATE_POSITION]() {
      state.panel.edge = state.panel.nextEdge
      state.panel.alignment = state.panel.nextAlignment
    },
    [T.NEXT_EDGE](edge) {
      state.panel.nextEdge = edge
    },
    [T.NEXT_ALIGNMENT](align) {
      state.panel.nextAlignment = align
    },
    [T.NEXT_BEHAVIOR](behavior) {
      state.panel.behavior = behavior
    },
    [T.UPDATING_POSITION](value) {
      state.panel.updatingPosition = value
    },
    [T.SHOW_PANEL]() {
      state.panel.visible = true
    },
    [T.HIDE_PANEL]() {
      state.panel.visible = false
    },
    [T.NEXT_TASK_COUNT1](count) {
      state.panel.nextTaskCount1 = count
    },
    [T.UPDATE_TASK_COUNT_1]() {
      state.panel.taskCount1 = state.panel.nextTaskCount1
    },
    [T.MASK_GROW](grow) {
      state.geometry.maskGrow = grow
    },
    [T.SHOW_SETTINGS](visible) {
      state.settings.visible = visible
    },
    [T.LOCK_VISIBLE](lock) {
      state.panel.lockVisible = lock
    }
  })
}
