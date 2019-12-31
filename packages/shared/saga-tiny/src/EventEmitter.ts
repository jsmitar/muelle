import genId from 'private/genId';

const id = genId();

export type Listener = ((...args: any) => any) & { [k: string]: any };

export type ListenerId = number;

export class EventEmitter {
  private listeners: {
    [event: string]: { [id: number]: Listener };
  } = {};

  private saveListener(event: string, listener: Listener) {
    if (!(event in this.listeners)) this.listeners[event] = {};
    listener._id_ = id.next().value;

    return (this.listeners[event][listener._id_] = listener);
  }

  emit(event: string, payload: any) {
    if (event in this.listeners) {
      Object.values(this.listeners[event]).forEach(listener =>
        listener(payload, event)
      );
    }
  }

  once(event: string, listener: Listener): ListenerId {
    const cb = this.saveListener(event, (payload: string) => {
      this.off(event, cb._id_);
      listener(payload);
    });

    return cb._id_;
  }

  off(event: string, listenerId: ListenerId) {
    delete this.listeners[event][listenerId];
  }

  offAll() {
    this.listeners = {};
  }
}
