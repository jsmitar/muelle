import Qt from 'qt';
import { delayed } from './baseEffects';

export function delay(ms: number) {
  return delayed(resolve => {
    const timer = Qt.setTimeout(resolve, ms);
    return () => Qt.clearTimeout(timer);
  });
}
