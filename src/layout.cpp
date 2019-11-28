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

#include "layout.hpp"

namespace Dock {

Layout::Layout(QObject *parent) : QObject(parent) {
  setObjectName(QStringLiteral("Dock::Layout"));
  connect(this, &Layout::edgeChanged, this, &Layout::layoutChanged);
  connect(this, &Layout::alignmentChanged, this, &Layout::layoutChanged);
}

Layout::~Layout() {}

void Layout::setEdge(Types::Edge edge) {
  switch (edge) {
  case Types::Edge::Top:
  case Types::Edge::Bottom:
    if (mEdge != edge) {
      mEdge = edge;
      if (mOrientation != Types::Orientation::Horizontal) {
        mOrientation = Types::Orientation::Horizontal;
        emit orientationChanged();
      }
      emit edgeChanged();
    }
    break;
  case Types::Edge::Left:
  case Types::Edge::Right:
    if (mEdge != edge) {
      mEdge = edge;
      if (mOrientation != Types::Orientation::Vertical) {
        mOrientation = Types::Orientation::Vertical;
        emit orientationChanged();
      }
      emit edgeChanged();
    }
    break;
  }
}

void Layout::setAlignment(Types::Alignment alignment) {
  if (mAlignment != alignment) {
    mAlignment = alignment;
    emit alignmentChanged();
  }
}
} // namespace Dock
