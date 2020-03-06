import {
  TaskItemModel,
  TasksModel,
} from 'plasma/org.kde.plasma.private.taskmanager-0.1';
import Qt from 'qt';

export function activate(
  tasksModel: TasksModel,
  { index, m }: { index: number; m: TaskItemModel },
  { modifiers }: { modifiers: Qt.KeyboardModifier }
) {
  const {
    makeModelIndex,
    requestToggleMinimized,
    requestActivate,
    requestNewInstance,
    requestPublishDelegateGeometry,
  } = tasksModel;

  const modelIndex = makeModelIndex(index);
  requestPublishDelegateGeometry(modelIndex, Qt.rect(0, 0, 0, 0));
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
