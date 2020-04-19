import type QtType from '../qt';

export default ({
  setTimeout: global.setTimeout,
  clearTimeout: global.clearTimeout,
  setInterval: global.setInterval,
  clearInterval: global.clearInterval,
  callLater: global.setImmediate,
} as any) as typeof QtType;
