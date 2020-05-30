import { Effect, Task } from 'qrc:/shared/saga/private/types';

export interface SagaMonitor extends SagaMonitorHooks {
  enable: boolean;
  [props: string]: any;
}

export interface SagaMonitorHooks {
  rootSagaStarted(task: Task): void;
  effectTriggered(task: Task, effect?: Effect, result?: any): void;
  effectResolved(task: Task, effect?: Effect, response?: any): void;
  statusChanged(task: Task, result?: any): void;
}
