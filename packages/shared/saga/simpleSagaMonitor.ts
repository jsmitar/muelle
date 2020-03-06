import { noop, tostr } from '../functional';
import { effectType, SELECT, TaskStatus } from './private/symbols';
import { SagaMonitor, SagaMonitorHooks } from './sagaMonitor';

const sagaMonitorHooks: SagaMonitorHooks = {
  rootSagaStarted(task) {
    console.info(`${task} [Started]`);
  },
  effectTriggered(task, effect?, result?) {
    const triggered = `\u001b[43m\u001b[30m[Triggered]\u001b[0m`;
    console.info(
      `${task} ${triggered} Effect: ${tostr(effect ?? 'none', 3, -1)}`,
      result !== undefined ? `Result: ${tostr(result, 3, -1)}` : ''
    );
  },
  effectResolved(task, effect, response) {
    const deep = effect?.[effectType] === SELECT ? 1 : 3;
    const resolved = `\u001b[45m\u001b[30m[Resolved]\u001b[0m`;
    console.info(
      `${task} ${resolved} Effect: ${tostr(effect ?? 'none', 3, -1)}`,
      response !== undefined
        ? `Response: ${tostr(response, deep, -1)}`.replace(
            / \w+Changed: \[Function\],?/g,
            ''
          )
        : ''
    );
  },
  statusChanged(task, result) {
    const status = `\u001b[42m\u001b[30m[${TaskStatus[task.status]}]\u001b[0m`;

    console.info(
      `${task} ${status}`,
      result !== undefined ? `Result: ${tostr(result, 3, -1)}` : ''
    );
  },
};

const sagaMonitorHooksDisabled: SagaMonitorHooks = {
  rootSagaStarted: noop,
  effectResolved: noop,
  effectTriggered: noop,
  statusChanged: noop,
};

export const simpleSagaMonitor: SagaMonitor = {
  _enable: false,

  get enable() {
    return this._enable;
  },
  set enable(value) {
    this._enable = value;
    if (this._enable) {
      Object.assign(this, { ...this, ...sagaMonitorHooks });
    } else {
      Object.assign(this, { ...this, ...sagaMonitorHooksDisabled });
    }
  },
  ...sagaMonitorHooksDisabled,
};
