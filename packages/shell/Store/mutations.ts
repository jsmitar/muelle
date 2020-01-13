import {
  ShellState,
  SLIDE_STATUS,
  UPDATE_POSITION,
  NEXT_EDGE,
  LOCK_VISIBLE,
  NEXT_ALIGNMENT,
  NEXT_BEHAVIOR,
  UPDATING_POSITION,
  SHOW_PANEL,
  HIDE_PANEL,
  NEXT_TASK_COUNT1,
  UPDATE_TASK_COUNT_1,
  MASK_GROW,
  SHOW_SETTINGS,
} from './types';
import { Mutations } from '../../shared/flux/flux';

export const mutations: Mutations<ShellState> = {
  [SLIDE_STATUS](state, status) {
    state.panel.slide = status;
  },
  [UPDATE_POSITION](state) {
    state.panel.edge = state.panel.nextEdge;
    state.panel.alignment = state.panel.nextAlignment;
  },
  [NEXT_EDGE](state, edge) {
    state.panel.nextEdge = edge;
  },
  [NEXT_ALIGNMENT](state, align) {
    state.panel.nextAlignment = align;
  },
  [NEXT_BEHAVIOR](state, behavior) {
    state.panel.behavior = behavior;
  },
  [UPDATING_POSITION](state, value) {
    state.panel.updatingPosition = value;
  },
  [SHOW_PANEL](state) {
    state.panel.visible = true;
  },
  [HIDE_PANEL](state) {
    state.panel.visible = false;
  },
  [NEXT_TASK_COUNT1](state, count) {
    state.panel.nextTaskCount1 = count;
  },
  [UPDATE_TASK_COUNT_1](state) {
    state.panel.taskCount1 = state.panel.nextTaskCount1;
  },
  [MASK_GROW](state, grow) {
    state.geometry.maskGrow = grow;
  },
  [SHOW_SETTINGS](state, visible) {
    state.settings.visible = visible;
  },
  [LOCK_VISIBLE](state, lock) {
    state.panel.lockVisible = lock;
  },
};
