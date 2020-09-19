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

#include "propertymap.hpp"

PropertyMap::PropertyMap(QObject *parent) : QQmlPropertyMap(parent) {}

QQmlListProperty<QObject> PropertyMap::children() {
  return {nullptr,
          &m_children,
          &PropertyMap::appendChild,
          &PropertyMap::childrenCount,
          &PropertyMap::child,
          &PropertyMap::clearChildren,
          &PropertyMap::replaceChild,
          &PropertyMap::removeLastChild};
}

void PropertyMap::appendChild(QQmlListProperty<QObject> *prop,
                              QObject *object) {

  auto m_children = *reinterpret_cast<QVector<QObject *> *>(prop->data);
  m_children.append(object);
}
int PropertyMap::childrenCount(QQmlListProperty<QObject> *prop) {
  auto m_children = *reinterpret_cast<QVector<QObject *> *>(prop->data);
  return m_children.count();
}
QObject *PropertyMap::child(QQmlListProperty<QObject> *prop, int index) {
  auto m_children = *reinterpret_cast<QVector<QObject *> *>(prop->data);
  return m_children[index];
}
void PropertyMap::clearChildren(QQmlListProperty<QObject> *prop) {
  auto m_children = *reinterpret_cast<QVector<QObject *> *>(prop->data);
  m_children.clear();
}
void PropertyMap::replaceChild(QQmlListProperty<QObject> *prop, int index,
                               QObject *object) {
  auto m_children = *reinterpret_cast<QVector<QObject *> *>(prop->data);
  m_children[index] = object;
}
void PropertyMap::removeLastChild(QQmlListProperty<QObject> *prop) {
  auto m_children = *reinterpret_cast<QVector<QObject *> *>(prop->data);
  m_children.removeLast();
}