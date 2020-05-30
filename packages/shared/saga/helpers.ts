import { tostr } from 'qrc:/shared/functional';
import { effectType } from 'qrc:/shared/saga/private/symbols';
import { Effect, Task } from 'qrc:/shared/saga/private/types';
import { TaskController } from 'qrc:/shared/saga/saga';

export function isTask(value: any): value is Task {
  return value instanceof TaskController;
}

export function isEffect(effect: any): effect is Effect {
  if (!effect || !(effectType in effect)) {
    throw new Error(
      `Saga: A Effect was expected but the value was: ${tostr(effect)}`
    );
  }

  return true;
}
