import genId from '../genId';
import { AssertionError } from 'assert';

const id = genId();

export type Listener<Args extends any[] = any[]> = (...payload: Args) => any;

export type ListenerId = number;

export type Connection = { event: string; id: ListenerId };

function assertEventName(event: any): asserts event {
  if (!event) {
    throw new AssertionError({ message: 'Event name is falsy' });
  }

  return event;
}

export class EventEmitter {
  private listeners: {
    [event: string]: { [id: number]: Listener };
  } = {};

  private saveListener(event: string, listener: Listener): Connection {
    if (!(event in this.listeners)) this.listeners[event] = {};
    const listenerId = id.next().value;
    this.listeners[event][listenerId] = listener;

    return { event, id: listenerId };
  }

  emit(event: string, ...payload: any[]) {
    if (event in this.listeners) {
      Object.values(this.listeners[event]).forEach(listener =>
        listener(...payload)
      );
    }
  }

  once<Args extends any[] = any[]>(
    event: string,
    listener: Listener<Args>
  ): Connection {
    assertEventName(event);

    const connection = this.saveListener(event, (...payload) => {
      this.off(connection);
      listener(...(payload as any));
    });

    return connection;
  }

  off({ event, id }: Connection) {
    delete this.listeners[event][id];
  }

  offAll(connections?: Connection[]) {
    if (connections) {
      for (const c of connections) this.off(c);
    } else {
      this.listeners = {};
    }
  }
}
