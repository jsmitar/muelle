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
#include "behavior.hpp"
#include "behavior_p.hpp"
#include "dockview.hpp"

namespace Dock {
Behavior::Behavior(View *parent) : dPtr(new BehaviorPrivate(this)) {

  connect(parent, &Dock::View::panelGeometryChanged,
          [this, parent]() { dPtr->setGeometry(parent->panelGeometry()); });
}

Behavior::~Behavior() { delete dPtr; }

Types::Behavior Behavior::behavior() const { return dPtr->behavior; }

void Behavior::setBehavior(Types::Behavior behavior) {
  dPtr->setBehavior(behavior);
  emit behaviorChanged();
}

bool Behavior::dodge() const { return dPtr->nextDodgeValue; }

void Behavior::setParentWindow(WId wid) const { dPtr->parentWId = wid; }
} // namespace Dock
