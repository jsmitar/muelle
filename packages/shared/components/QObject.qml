import QtQuick 2.14
import '../functional.ts' as F

QtObject {
  id: object
  objectName: 'QObject'

  property QtObject parentObject: null
  default property list<QtObject> resources

  onResourcesChanged: {
    F.each(resources, res => {
      if ('parentObject' in res && !res.parentObject) {
        res.parentObject = object
      }
    })
  }
}
