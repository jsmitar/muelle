import Qt from 'qt';
import { Timer } from '@qml/QtQml-2.x';

(function(global: any) {
  const timerComponent = Qt.createComponent<Timer>('TimerComponent.qml');
  const timers: Record<number, Timer> = {};
  let nextId = 0;

  function clearTimeout(id: number) {
    if (!timers.hasOwnProperty(id)) return;

    const timer = timers[id];
    timer.stop();
    timer.destroy();
    delete timers[id];
  }

  function setTimeout(callback: () => void, interval = 0) {
    const timer = timerComponent.createObject(null, {
      interval,
      repeat: false,
    });

    if (!timer) throw new Error('Fail to create Timer Component');

    const id = ++nextId;
    timers[id] = timer;

    timer.triggered.connect(() => {
      try {
        callback();
      } catch (e) {
        console.warn(e);
      } finally {
        timer.destroy();
        delete timers[id];
      }
    });
    timer.running = true;

    return id;
  }

  function setInterval(callback: () => void, interval = 0) {
    const timer = timerComponent.createObject(null, { interval, repeat: true });

    if (!timer) throw new Error('Fail to create Timer Component');

    const id = ++nextId;
    timers[id] = timer;

    timer.triggered.connect(() => {
      try {
        callback();
      } catch (e) {
        console.warn(e);
      }
    });
    timer.running = true;

    return id;
  }

  global.setTimeout = setTimeout;
  global.clearTimeout = clearTimeout;

  global.setInterval = setInterval;
  global.clearInterval = clearTimeout;
})(Qt);
