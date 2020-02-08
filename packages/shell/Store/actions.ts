import Muelle from '@qml/org.muelle.types-1.0';
import { createAction } from '../../shared/flux/createAction';

export const slideIn = createAction('slide', 'in');

export const slideOut = createAction('slide', 'out');

export const slideInFinished = createAction('slide-in-finished');

export const slideOutFinished = createAction('slide-out-finished');

export const changeAlignment = createAction<Muelle.Types.Alignment>(
  'changeAlignment'
);

export const changeEdge = createAction<Muelle.Types.Edge>('edge');

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
