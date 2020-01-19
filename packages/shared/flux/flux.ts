import Qt from 'qt';
import { debounce, tostr } from '../functional';
import EventEmitter from '../saga/eventEmitter';
import { SagaFn, Task } from '../saga/private/types';
import { runSaga } from '../saga/saga';
import { SagaMonitor } from '../saga/sagaMonitor';

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
  sagaMonitor?: SagaMonitor;
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
  debouncedDispatch: Record<string, (action: Action) => void> = {};

  constructor(options: Options<S, M>) {
    (<any>Qt).store = this;
    assertOptions(options);
    this.state = options.state;
    this.mutations = options.mutations;
    this.sagaTaskRoot = runSaga(
      {
        commit: (...args) => this.commit(...args),
        actionSubscriber: this.actionEmitter,
        getState: () => this.state,
        sagaMonitor: options.sagaMonitor,
      },
      options.sagaRoot
    );
  }

  commit(type: string, ...args: any[]) {
    try {
      console.info(`[COMMIT]: ${tostr({ type, args }, 3, -1)}`);
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
    if (action.type in this.debouncedDispatch) {
      this.debouncedDispatch[action.type](action);
    } else {
      this.debouncedDispatch[action.type] = debounce((action: Action) => {
        this.actionEmitter.emit(action.type, action);
      });
      this.debouncedDispatch[action.type](action);
    }
  }
}
