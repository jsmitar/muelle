#ifndef PROPERTYMAP_HPP
#define PROPERTYMAP_HPP

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>
#include <QQmlParserStatus>
#include <QQmlPropertyMap>
#include <qglobal.h>

class PropertyMap : public QQmlPropertyMap, public QQmlParserStatus {
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)
public:
  explicit PropertyMap(QObject *parent = nullptr);

  virtual void classBegin() {}
  virtual void componentComplete() {}

private:
};

inline void qmlRegisterPropertyMap() {
  qmlRegisterType<PropertyMap>("org.duckdock.extra", 1, 0, "PropertyMap");
}

#endif // PROPERTYMAP_HPP
