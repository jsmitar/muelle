import Qt from 'qt';
import EventEmitter from 'shared/saga-tiny/eventEmitter';
import { SagaFn, Task } from 'shared/saga-tiny/private/types';
import { run } from 'shared/saga-tiny/saga';

export type State<T = any> = Qt.QtObject<T>;
export type Mutations<S = any> = Record<
  string,
  (state: S, payload: any) => void
>;

export type Action = { type: string; payload?: any };

export type Options<S extends State, M extends Mutations<S>> = {
  state: S;
  mutations: M;
  sagaRoot: SagaFn;
};

class FluxStore<S extends State, M extends Mutations> {
  state: S;
  mutations: M;
  actionEmitter = new EventEmitter();
  sagaTaskRoot: Task;

  constructor(options: Options<S, M>) {
    (<any>Qt).store = this;
    this.state = options.state;
    this.mutations = options.mutations;
    this.sagaTaskRoot = run(
      {
        commit: this.commit,
        actionSubscriber: this.actionEmitter,
        getState: () => this.state,
      },
      options.sagaRoot
    );
  }

  commit(type: string, payload: any) {
    try {
      if (type in this.mutations) {
        this.mutations[type](this.state, payload);
      } else {
        throw new Error(`FluxStore.commit: fail to call mutation: ${type}`);
      }
    } catch (e) {
      console.error((<Error>e).message, <Error>e.stack);
    }
  }

  dispatch(action: Action) {
    this.actionEmitter.emit(action.type, action);
  }
}

export default FluxStore;
