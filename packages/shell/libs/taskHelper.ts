//@pragma library

function activate(
  taskModel,
  { index, m: { IsActive, IsMinimized } }, // TaskItemModel
  { modifiers }
) {
  // MouseArea
  const {
    makeModelIndex,
    requestToggleMinimized,
    requestActivate,
    requestNewInstance,
  } = taskModel;

  const modelIndex = makeModelIndex(index);
  if (modifiers === Qt.NoModifier) {
    if (IsActive) {
      requestToggleMinimized(modelIndex);
    } else {
      if (IsMinimized) {
        requestToggleMinimized(modelIndex);
      }
      requestActivate(modelIndex);
    }
  }
  if (modifiers & Qt.ShiftModifier) {
    requestNewInstance(modelIndex);
  }
}
