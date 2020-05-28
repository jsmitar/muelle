import QtQml from '@qml/QtQml-2.14';
import Qt, { QtObject } from 'qt';
import { qassign } from '../../shared/functional';

export type Destroy = () => void;

type Cache = {
  object: Qt.QtObject<any>;
  d_timer?: Qt.Timer;
};

class MemoComponent {
  objectCache: Record<string, Cache> = {};
  parent: QtObject;

  constructor(parent: QtObject) {
    this.parent = parent;
  }

  createObject(
    component: QtQml.Component<any>,
    objectName: string,
    props: Record<any, any> = {}
  ) {
    if (objectName in this.objectCache) {
      const cache = this.objectCache[objectName];
      this.cancelDestroy(cache);

      qassign(cache.object, props);

      return this.objectCache[objectName].object;
    }

    const object = component.createObject(this.parent);

    if (!object) return null;
    qassign(object, { ...props, objectName });

    this.objectCache[objectName] = { object };

    return object;
  }

  cancelDestroy(cache: Cache) {
    if (cache.d_timer) {
      cache.d_timer = void Qt.clearTimeout(cache.d_timer);
    }
  }

  destroyLater(objectName: string) {
    const cache = this.objectCache[objectName];
    if (cache) {
      cache.d_timer = Qt.setTimeout(() => {
        this.destroy(objectName);
      }, 1000);
    }
  }

  destroy(objectName: string) {
    const cache = this.objectCache[objectName];
    if (cache) {
      delete this.objectCache[objectName];
      if (cache.d_timer) Qt.clearTimeout(cache.d_timer);
      cache.object.destroy();
    }
  }

  destroyAll() {
    for (const objectName in this.objectCache) {
      if (this.objectCache.hasOwnProperty(objectName)) this.destroy(objectName);
    }
  }
}

export function createMemoComponent(cleanup: Qt.Signal, parent: QtObject) {
  const memo = new MemoComponent(parent);
  cleanup.connect(() => memo.destroyAll());

  return memo;
}
