export class EventEmitter {
  constructor() {
    this.listeners = { '*': {} };
    this.nextId = 0;
  }
  emit(event, payload) {
    if (event in this.listeners) {
      Object.values(this.listeners[event]).forEach(listener =>
        listener(payload, event)
      );
    }
    Object.values(this.listeners['*']).forEach(listener =>
      listener(payload, event)
    );
  }
  all(events, listener) {
    const payloads = new Map();
    const cb = (payload, event) => {
      payloads.set(event, payload);
      if (payloads.size === events.length) {
        events.forEach(event => this.off(event, cb));
        listener(payloads.values());
      }
    };
    cb._id_ = ++this.nextId;
    events.forEach(event => {
      if (!(event in this.listeners)) this.listeners[event] = {};
      this.listeners[event][cb._id_] = cb;
    });
    return () => events.forEach(event => this.off(event, cb));
  }
  race(events, listener) {
    const cb = (payload, event) => {
      events.forEach(event => this.off(event, cb));
      const payloads = Array.from({ length: events.length });
      payloads.splice(events.indexOf(event), 1, payload);
      listener(payloads, event);
    };
    cb._id_ = ++this.nextId;
    events.forEach(event => {
      if (!(event in this.listeners)) this.listeners[event] = {};
      this.listeners[event][cb._id_] = cb;
    });
    return () => events.forEach(event => this.off(event, cb));
  }
  once(event, listener) {
    if (!(event in this.listeners)) this.listeners[event] = {};
    const cb = payload => {
      this.off(event, cb);
      listener(payload);
    };
    cb._id_ = ++this.nextId;
    this.listeners[event][cb._id_] = cb;
    return () => this.off(event, cb);
  }
  off(event, listener) {
    delete this.listeners[event][listener._id_];
  }
}
