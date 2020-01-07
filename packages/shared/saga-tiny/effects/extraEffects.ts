import { delayed } from './baseEffects';

export function delay(time: number) {
  return delayed(resolve => {
    const timer = setTimeout(resolve, time);
    return () => clearTimeout(timer);
  });
}
