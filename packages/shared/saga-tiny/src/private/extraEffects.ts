import { delayed } from './baseEffects';

export function delay(time: number) {
  return delayed(resolve => {
    setTimeout(resolve, time);
  });
}
