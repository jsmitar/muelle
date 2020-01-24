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

#ifndef Types_HPP
#define Types_HPP

#include <QObject>
#include <QQmlEngine>

namespace Muelle { // namespace Muelle
class Types {
  Q_GADGET
  Q_DISABLE_COPY(Types)

public:
  enum class Edge {
    Top = 1 << 1,
    Right = 1 << 2,
    Bottom = 1 << 3,
    Left = 1 << 4,
  };
  Q_ENUM(Edge)

  enum class Orientation {
    Horizontal = 1 << 5,
    Vertical = 1 << 6,
  };
  Q_ENUM(Orientation)

  enum class Alignment {
    Start = 1 << 7,
    Center = 1 << 8,
    End = 1 << 9,
  };
  Q_ENUM(Alignment)

  enum class Behavior { None, AlwaysVisible, AutoHide, DodgeAll, DodgeActive };
  Q_ENUM(Behavior)
};

inline void qmlRegisterTypes() {
  qmlRegisterUncreatableType<Types>("org.muelle.types", 1, 0, "Types",
                                    "Muelle::Types is an uncreatable type");
}
} // namespace Muelle
#endif // Types_HPP
