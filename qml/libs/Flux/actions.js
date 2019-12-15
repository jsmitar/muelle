.pragma library

function showPanel() {
  return { type: 'showPanel' }
}

function hidePanel() {
  return { type: 'hidePanel' }
}

function changeAlignment(alignment) {
  return { type: 'changeAlignment', payload: alignment }
}

function changeEdge(edge) {
  return { type: 'changeEdge', payload: edge }
}

function changeBehavior(behavior) {
  return { type: 'changeBehavior', payload: behavior }
}

function showSettings(visible) {
  return { type: 'showSettings', payload: visible }
}

function maskGrow(duration) {
  return { type: 'maskGrow', payload: duration }
}

function updateTaskCount1(count) {
  return { type: 'updateTaskCount1', payload: count }
}

function updateLayout() {
  return { type: 'updateLayout' }
}
