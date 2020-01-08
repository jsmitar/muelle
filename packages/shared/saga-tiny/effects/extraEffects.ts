import Qt from 'qt';
import { delayed } from './baseEffects';

export function delay(time: number) {
  return delayed(resolve => {
    const timer = Qt.setTimeout(resolve, time);
    return () => Qt.clearTimeout(timer);
  });
}
