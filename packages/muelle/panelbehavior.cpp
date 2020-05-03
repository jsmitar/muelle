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
    : QObject(parent), dPtr(new Private::PanelBehavior(this)) {}

PanelBehavior::~PanelBehavior() { delete dPtr; }

void PanelBehavior::classBegin() {}

void PanelBehavior::componentComplete() {
  connect(this, &PanelBehavior::behaviorChanged, dPtr,
          &Private::PanelBehavior::updateBehavior);
  dPtr->updateBehavior();
}

Types::Behavior PanelBehavior::behavior() const noexcept { return mBehavior; }

void PanelBehavior::setBehavior(Types::Behavior behavior) noexcept {
  if (mBehavior != behavior) {
    mBehavior = behavior;
    emit behaviorChanged();
  }
}

void PanelBehavior::setPosition(const QPoint &position) noexcept {
  dPtr->position = position;
  emit positionChanged();
}

QPoint PanelBehavior::position() const noexcept { return dPtr->position; }

void PanelBehavior::setRegion(const QVariant &rects) noexcept {
  const QJSValue array = rects.value<QJSValue>();

  if (!array.isArray()) {
    qWarning() << "value is not array";
    return;
  }
  QRegion region;

  for (int i = 0; i < array.property(QStringLiteral("length")).toInt(); ++i) {
    region += array.property(i).toVariant().toRect();
  }
  dPtr->region = region;

  emit regionChanged();
}

QVariant PanelBehavior::region() const noexcept { return dPtr->region; }

bool PanelBehavior::dodge() const noexcept { return dPtr->dodgeValue; }

void PanelBehavior::updateStruts() noexcept { dPtr->updateStruts(); }

void PanelBehavior::scanWindows() noexcept { dPtr->scanAllWindows(); }

} // namespace Muelle
