import QtQuick 2.10
import "../../Extras"
import "./types.js" as T
import "../functional.js" as F
import "../signal.js" as Signal
import "./actions.js" as Action

QObject {
  property FluxStore store

  property FluxState state: store.state
  property var commit: store.commit
  property var dispatch: store.dispatch

  function hidePanel() {
    const { slide } = root

    if (state.panel.hidden && state.panel.slide !== 'Running') return;

    return new Qt.Promise((resolve, reject) => {
      slide.hide.cancel()
        .start(() => {
          commit(T.SLIDE_STATUS, 'Running')
        })
        .completed(() => {
          commit(T.HIDE_PANEL)
          $view.setOpacity(0)
          commit(T.UPDATE_TASK_COUNT_1)
          $positioner.update(0)
          commit(T.SLIDE_STATUS, 'None')
          resolve()
        })
        .cancelled(reject)
    })
  }

  function showPanel() {
    const { slide } = root

    if (state.panel.visible && state.panel.slide !== 'Running') return;

    return new Qt.Promise((resolve, reject) => {
      slide.show.cancel()
        .start(() => {
          $view.setOpacity(1)
          commit(T.SLIDE_STATUS, 'Running')
        })
        .completed(() => {
          commit(T.SHOW_PANEL)
          commit(T.SLIDE_STATUS, 'None')
          resolve()
        })
        .cancelled(reject)
    })
  }

  function updateLayout() {
    commit(T.UPDATING_POSITION, true)
    return dispatch(Action.hidePanel())
      .then(() => {
        commit(T.UPDATE_POSITION)
        $positioner.update(0)
      })
      .then(() => {
        return dispatch(Action.showPanel())
      })
      .then(() => commit(T.UPDATING_POSITION, false))
  }

  function changeEdge(edge) {
    commit(T.NEXT_EDGE, edge)
    return dispatch(Action.updateLayout())
  }

  function changeAlignment(align) {
    commit(T.NEXT_ALIGNMENT, align)
    return dispatch(Action.updateLayout())
  }

  function changeBehavior(behavior) {
    commit(T.NEXT_BEHAVIOR, behavior)
  }

  function maskGrow(duration) {
    if (Qt.cancelMaskGrow)
      Qt.cancelMaskGrow()

    if (!store.state.geometry.maskGrow)
      commit(T.MASK_GROW, true)

    const { cancel, job } = F.makeCancelable(() => F.asyncTimeout(duration))
    Qt.cancelMaskGrow = cancel

    return job()
      .then(() => commit(T.MASK_GROW, false))
      .catch(F.noop)
  }

  function updateTaskCount1(count) {
    commit(T.NEXT_TASK_COUNT1, count)
    if (count < state.panel.taskCount1) {
      return dispatch(Action.maskGrow(state.animation.duration))

    } else if (count !== state.panel.taskCount1) {
      commit(T.UPDATE_TASK_COUNT_1)
    }
  }

  function showSettings(visible) {
    if (visible !== store.state.settings.visible) {
      commit(T.SHOW_SETTINGS, visible)
      lockVisible(visible)
    }
  }

  function lockVisible(lock) {
    commit(T.LOCK_VISIBLE, lock)
    if (lock)
      return store.dispatch(Action.showPanel())
  }
}
