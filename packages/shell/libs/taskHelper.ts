'.pragma library';

import Qt from 'qt';
import { TaskItemModel } from 'plasma/org.kde.plasma.private.taskmanager-0.1';

export function activate(
  taskModel: any,
  { index, m }: { index: number; m: TaskItemModel },
  { modifiers }: { modifiers: Qt.KeyboardModifier }
) {
  const {
    makeModelIndex,
    requestToggleMinimized,
    requestActivate,
    requestNewInstance,
  } = taskModel;

  const modelIndex = makeModelIndex(index);
  if (modifiers === Qt.NoModifier) {
    if (m.IsActive) {
      requestToggleMinimized(modelIndex);
    } else {
      if (m.IsMinimized) {
        requestToggleMinimized(modelIndex);
      }
      requestActivate(modelIndex);
    }
  }
  if (modifiers & Qt.ShiftModifier) {
    requestNewInstance(modelIndex);
  }
}
