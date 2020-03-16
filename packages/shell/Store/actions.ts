import Muelle from '@qml/org.muelle.types-1.0';
import Qt from 'qt';
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

export const updateTaskCount1 = createAction(
  'updateTaskCount1',
  ({ launcherList, tasks }: { launcherList: string[]; tasks: number }) => {
    const separators = launcherList.filter(
      item => item === Qt.__muelle_separator__
    ).length;

    return {
      tasks: tasks - separators,
      separators,
    };
  }
);

export const updateLayout = createAction('updateLayout');

export const syncLaunchers = createAction('syncLaunchers');
