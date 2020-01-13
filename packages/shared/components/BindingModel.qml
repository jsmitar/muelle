import QtQuick 2.12

QObject {
  id: model
  property QtObject source
  property string sourceProperty
  property alias property: model.sourceProperty

  property QtObject target
  property string targetProperty: sourceProperty

  property bool when: true
  property bool _enabled: false;

  Binding {
    target: model.source
    property: sourceProperty
    value: model.target[targetProperty]
    when: model.when && model._enabled
    delayed: true
  }

  Binding {
    target: model.target
    property: targetProperty
    value: model.source[sourceProperty]
    when: model.when && model._enabled
    delayed: true
  }

  Component.onCompleted: {
    target[targetProperty] = source[sourceProperty]
    _enabled = true
  }
}
