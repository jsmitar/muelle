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

#ifndef HELPERS_HPP
#define HELPERS_HPP

#include <QJSValue>
#include <QObject>
#include <QQmlEngine>
#include <QVariant>
#include <qglobal.h>

namespace Dock {
class Helpers : public QObject {
  Q_OBJECT
public:
  explicit Helpers(QObject *parent = nullptr);

  virtual ~Helpers() override;

  Q_INVOKABLE static bool qReflectSignal(const QVariant &fn);
};

} // namespace Dock
#endif // HELPERS_HPP
