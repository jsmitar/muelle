import Muelle from '@qml/org.muelle.types-1.0';
import { createAction } from '../../shared/flux/createAction';

export const slide = 'slide';

export const slideIn = createAction(slide, 'in');

export const slideOut = createAction(slide, 'out');

export const slideInFinished = createAction('slide-in-finished');

export const slideOutFinished = createAction('slide-out-finished');

export const changeAlignment = createAction<Muelle.Types.Alignment>(
  'changeAlignment'
);

export const changeEdge = createAction<Muelle.Types.Edge>('edge');

export const changeBehavior = createAction<Muelle.Types.Behavior>(
  'changeBehavior'
);

export const showSettings = createAction<boolean>('showSettings');

export const maskGrow = createAction</* duration: */ number>('maskGrow');

export const disableMask = createAction('disableMask');

export const updateTaskCount1 = createAction<number>('updateTaskCount1');

export const updateLayout = createAction('updateLayout');

export const syncLaunchers = createAction('syncLaunchers');
