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
#include "panelbehavior.hpp"
#include "dockview.hpp"
#include "panelbehavior_p.hpp"

namespace Muelle {
PanelBehavior::PanelBehavior(QObject *parent)
    : QObject(parent), dPtr(new Private::PanelBehavior(this)) {}

PanelBehavior::~PanelBehavior() { delete dPtr; }

void PanelBehavior::classBegin() {}

void PanelBehavior::componentComplete() {
  connect(this, &PanelBehavior::behaviorChanged, dPtr,
          &Private::PanelBehavior::updateBehavior);
  dPtr->updateBehavior();
}

Types::Behavior PanelBehavior::behavior() const { return mBehavior; }

void PanelBehavior::setBehavior(Types::Behavior behavior) {
  if (mBehavior != behavior) {
    mBehavior = behavior;
    emit behaviorChanged();
  }
}

void PanelBehavior::updateStruts() { dPtr->updateStruts(); }

bool PanelBehavior::dodge() const { return dPtr->dodgeValue; }

} // namespace Muelle
