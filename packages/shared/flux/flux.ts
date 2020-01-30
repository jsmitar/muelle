import Qt from 'qt';
import { A } from 'ts-toolbelt';
import { DeepReadonly } from '../DeepReadonly';
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

export type ActionAny = {
  type: string;
  payload?: any;
  meta?: any;
};

export type Action<P = unknown, M = void> = A.Compute<
  ActionPayload<P> & ActionMeta<M>
>;

export type ActionPayload<P = undefined> = { type: string; payload: P };

export type ActionMeta<M = undefined> = M extends void
  ? { type: string }
  : { type: string; meta: M };

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
  debouncedDispatch: Record<string, (action: ActionAny) => void> = {};

  constructor(options: Options<S, M>) {
    (<any>Qt).store = this;
    assertOptions(options);
    this.state = options.state;
    this.mutations = options.mutations;
    this.sagaTaskRoot = runSaga(
      {
        sagaMonitor: options.sagaMonitor,
        actionSubscriber: this.actionEmitter,
        dispatch: action => this.dispatch(action),
        commit: (...args) => this.commit(...args),
        getState: () => this.getState(),
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

  dispatch(action: ActionAny) {
    if (!(action.type in this.debouncedDispatch)) {
      this.debouncedDispatch[action.type] = debounce((action: ActionAny) => {
        this.actionEmitter.emit(action.type, action);
      });
    }
    this.debouncedDispatch[action.type](action);
  }

  getState(): DeepReadonly<S> {
    return this.state;
  }
}
