/* Muelle
 * Copyright (C) 2019 Smith Ar
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#ifndef PROPERTYMAP_HPP
#define PROPERTYMAP_HPP

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>
#include <QQmlListProperty>
#include <QQmlParserStatus>
#include <QQmlPropertyMap>
#include <QVector>
#include <qglobal.h>

class PropertyMap : public QQmlPropertyMap, public QQmlParserStatus {
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(QQmlListProperty<QObject> children READ children NOTIFY
                 childrenChanged DESIGNABLE false)

  Q_CLASSINFO("DefaultProperty", "children")

public:
  explicit PropertyMap(QObject *parent = nullptr);

  template <class DerivedType>
  PropertyMap(DerivedType *derived, QObject *parent)
      : QQmlPropertyMap(derived, parent) {}

private:
  QQmlListProperty<QObject> children();
  static void appendChild(QQmlListProperty<QObject> *, QObject *);
  static int childrenCount(QQmlListProperty<QObject> *);
  static QObject *child(QQmlListProperty<QObject> *, int);
  static void clearChildren(QQmlListProperty<QObject> *);
  static void replaceChild(QQmlListProperty<QObject> *, int, QObject *);
  static void removeLastChild(QQmlListProperty<QObject> *);

  virtual void classBegin() {}
  virtual void componentComplete() {}

signals:
  void childrenChanged();

private:
  QVector<QObject *> m_children;
};

inline void qmlRegisterPropertyMap() {
  qmlRegisterType<PropertyMap>("org.muelle.extra", 1, 0, "PropertyMap");
}

#endif // PROPERTYMAP_HPP
