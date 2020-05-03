import {
  TaskItemModel,
  TasksModel,
} from 'plasma/org.kde.plasma.private.taskmanager-0.1';
import Qt from 'qt';

export function publishGeometry(
  tasksModel: TasksModel,
  viewPosition: Qt.point,
  task: Qt.QtObject<any>
) {
  const modelIndex = tasksModel.makeModelIndex(task.index);
  const rect = task.mapToItem(
    null,
    viewPosition.x,
    viewPosition.y,
    task.width,
    task.height
  );

  tasksModel.requestPublishDelegateGeometry(modelIndex, rect);
}

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
  } = tasksModel;

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
