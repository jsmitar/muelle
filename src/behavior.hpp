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
#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "docktypes.hpp"

#include <QObject>
#include <QRect>
#include <QtGui/qwindowdefs.h>

namespace Dock {
class View;
class BehaviorPrivate;

class Behavior : public QObject {
  Q_OBJECT
  Q_PROPERTY(Dock::Types::Behavior behavior READ behavior WRITE setBehavior
                 NOTIFY behaviorChanged)
  Q_PROPERTY(bool dodge READ dodge NOTIFY dodgeChanged)

public:
  explicit Behavior(View *parent = nullptr);
  ~Behavior();

  Types::Behavior behavior() const;
  void setBehavior(Types::Behavior behavior);

  bool dodge() const;

  void setParentWindow(WId wid) const;

signals:
  void behaviorChanged();
  void dodgeChanged();
  void showingDesktop(bool showing);

private:
  BehaviorPrivate *dPtr;
};

} // namespace Dock

#endif // BEHAVIOR_H
