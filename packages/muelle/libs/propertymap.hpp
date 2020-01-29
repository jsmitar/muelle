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
#include <QQmlParserStatus>
#include <QQmlPropertyMap>
#include <qglobal.h>

class PropertyMap : public QQmlPropertyMap, public QQmlParserStatus {
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)

public:
  explicit PropertyMap(QObject *parent = nullptr);

  template <class DerivedType>
  PropertyMap(DerivedType *derived, QObject *parent)
      : QQmlPropertyMap(derived, parent) {}

  virtual void classBegin() {}
  virtual void componentComplete() {}
};

inline void qmlRegisterPropertyMap() {
  qmlRegisterType<PropertyMap>("org.muelle.extra", 1, 0, "PropertyMap");
}

#endif // PROPERTYMAP_HPP
