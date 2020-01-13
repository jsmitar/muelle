import Qt from 'qt';
import EventEmitter from '../saga-tiny/eventEmitter';
import { SagaFn, Task } from '../saga-tiny/private/types';
import { runSaga } from '../saga-tiny/saga';

export type State<T = any> = Qt.QtObject<T>;
export type Mutations<S = any> = Record<
  string,
  (state: S, ...args: any[]) => void
>;

export type Action<T = any> = { type: string; payload: T };

export type Options<S extends State, M extends Mutations<S>> = {
  state: S;
  mutations: M;
  sagaRoot: SagaFn;
};

function assertOptions(options: Options<any, any>): asserts options {
  if (!Qt.isQtObject(options.state)) {
    throw new Error('FluxStore: options.state is not instace of QtObject');
  }
  if (!options.mutations) {
    throw new Error('FluxStore: options.mutations is undefined');
  }
}

export class FluxStore<S extends State, M extends Mutations> {
  state: S;
  mutations: M;
  actionEmitter = new EventEmitter();
  sagaTaskRoot: Task;

  constructor(options: Options<S, M>) {
    (<any>Qt).store = this;
    assertOptions(options);
    this.state = options.state;
    this.mutations = options.mutations;
    this.sagaTaskRoot = runSaga(
      {
        commit: this.commit,
        actionSubscriber: this.actionEmitter,
        getState: () => this.state,
      },
      options.sagaRoot
    );
  }

  commit(type: string, ...args: any[]) {
    try {
      if (type in this.mutations) {
        this.mutations[type](this.state, ...args);
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
