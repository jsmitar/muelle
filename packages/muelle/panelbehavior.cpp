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
PanelBehavior::PanelBehavior(QObject *parent) noexcept
    : QObject(parent), d_ptr(new Private::PanelBehavior(this)) {}

PanelBehavior::~PanelBehavior() { delete d_ptr; }

void PanelBehavior::classBegin() {}

void PanelBehavior::componentComplete() {
  connect(this, &PanelBehavior::behaviorChanged, d_ptr,
          &Private::PanelBehavior::updateBehavior);
  d_ptr->updateBehavior();
}

void PanelBehavior::setView(View *view) { m_view = view; }

View *PanelBehavior::view() const { return m_view; }

bool PanelBehavior::dodge() const noexcept { return d_ptr->dodgeValue; }

Types::Behavior PanelBehavior::behavior() const noexcept { return m_behavior; }

void PanelBehavior::setBehavior(Types::Behavior behavior) noexcept {
  if (m_behavior != behavior) {
    m_behavior = behavior;
    emit behaviorChanged();
  }
}

void PanelBehavior::updateStruts() noexcept { d_ptr->updateStruts(); }

void PanelBehavior::scanWindows() noexcept { d_ptr->scanAllWindows(); }

} // namespace Muelle
