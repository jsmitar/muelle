import { Effect, Task } from './private/types';

export interface SagaMonitor {
  rootSagaStarted(task: Task): void;
  effectTriggered(task: Task, effect?: Effect): void;
  effectResolved(task: Task, effect?: Effect, response?: any): void;
  sagaStatusChanged(task: Task, result?: any): void;
}
