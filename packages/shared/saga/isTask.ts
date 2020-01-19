import { Task } from './private/types';
import { TaskController } from './saga';

export function isTask(value: any): value is Task {
  return value instanceof TaskController;
}
