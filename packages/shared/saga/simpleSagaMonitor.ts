import { tostr } from '../functional';
import { TaskStatus } from './private/symbols';
import { SagaMonitor } from './sagaMonitor';

export const simpleSagaMonitor: SagaMonitor = {
  rootSagaStarted(task) {
    console.info(`${task} [Started]`);
  },
  effectTriggered(task, effect?, result?) {
    console.info(
      `${task} [Triggered] Effect: ${tostr(effect ?? 'none', 3, -1)}`,
      result !== undefined ? `Result: ${tostr(result, 3, -1)}` : ''
    );
  },
  effectResolved(task, effect, response) {
    console.info(
      `${task} [Resolved] Effect: ${tostr(effect ?? 'none', 3, -1)}`,
      response !== undefined ? `Response: ${tostr(response, 3, -1)}` : ''
    );
  },
  sagaStatusChanged(task, result) {
    console.info(
      `${task} [${TaskStatus[task.status]}]`,
      result !== undefined ? `Result: ${tostr(result, 3, -1)}` : ''
    );
  },
};
