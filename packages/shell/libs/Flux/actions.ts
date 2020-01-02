'.pragma library';

import Muelle from '@qml/org.muelle.types-1.0';

export function showPanel() {
  return { type: 'showPanel' };
}

export function hidePanel() {
  return { type: 'hidePanel' };
}

export function changeAlignment(alignment: Muelle.Types.Alignment) {
  return { type: 'changeAlignment', payload: alignment };
}

export function changeEdge(edge: Muelle.Types.Edge) {
  return { type: 'changeEdge', payload: edge };
}

export function changeBehavior(behavior: Muelle.Types.Behavior) {
  return { type: 'changeBehavior', payload: behavior };
}

export function showSettings(visible: boolean) {
  return { type: 'showSettings', payload: visible };
}

export function maskGrow(duration: number) {
  return { type: 'maskGrow', payload: duration };
}

export function updateTaskCount1(count: number) {
  return { type: 'updateTaskCount1', payload: count };
}

export function updateLayout() {
  return { type: 'updateLayout' };
}
