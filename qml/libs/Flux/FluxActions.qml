import QtQuick 2.10
import "../../Extras"
import "./types.js" as T
import "../functional.js" as F
import "../signal.js" as Signal

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
    return dispatch({ type: 'hidePanel' })
      .then(() => {
        commit(T.UPDATE_POSITION)
        $positioner.update(0)
      })
      .then(() => {
        return dispatch({ type: 'showPanel' })
      })
      .then(() => commit(T.UPDATING_POSITION, false))
  }

  function changeEdge(edge) {
    commit(T.NEXT_EDGE, edge)
    return dispatch({ type: 'updateLayout' })
  }

  function changeAlignment(align) {
    commit(T.NEXT_ALIGNMENT, align)
    return dispatch({ type: 'updateLayout' })
  }

  Timer {
    id: maskGrowTimer;
    repeat: false;

    property var _promiseReject: () => {}
  }

  function maskGrow({duration}) {
    commit(T.MASK_GROW, true)
    return new Qt.Promise((resolve, reject) => {
      Signal.disconnectAll(maskGrowTimer.triggered)
      maskGrowTimer.stop()
      maskGrowTimer._promiseReject()
      maskGrowTimer._promiseReject = reject
      maskGrowTimer.interval = duration
      Signal.once(maskGrowTimer.triggered, resolve)
      maskGrowTimer.restart()
    })
    .then(() => {
      commit(T.MASK_GROW, false)
    })
  }

  function updateTaskCount1(count) {
    commit(T.NEXT_TASK_COUNT1, count)
    if (count < state.panel.taskCount1) {
      return dispatch({
          type: 'maskGrow',
          payload: { duration: state.animation.duration }
        })
    } else if (count !== state.panel.taskCount1) {
      commit(T.UPDATE_TASK_COUNT_1)
    }
  }
}
