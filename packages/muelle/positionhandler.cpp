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

#include "positionhandler.hpp"
#include "dockview.hpp"
#include <QPropertyAnimation>

namespace Muelle {
PositionHandler::PositionHandler(View *view)
    : mView(view), animation(new QPropertyAnimation(view)) {
  animation->setTargetObject(mView);
  animation->setPropertyName("position");
}

PositionHandler::~PositionHandler() {}

void PositionHandler::update(int duration) {
  if (auto screen = mView->screen(); screen) {
    if (mView->compositing()) {
      const auto newPosition = computePosition(
          screen->geometry(), {{0, 0}, mView->size()}, mView->layout());

      mView->setPosition(newPosition);
    } else {
      const auto newPosition = computePosition(
          screen->geometry(), mView->panelGeometry(), mView->layout());
      if (duration > 0) {
        animation->setDuration(duration);
        animation->setStartValue(mView->position());
        animation->setEndValue(newPosition);
        animation->start();
      } else {
        animation->stop();
        animation->setDuration(0);
        mView->setPosition(newPosition);
      }
    }
  }
}

void PositionHandler::setCenterOffset(float offset) {
  mOffset = std::min(std::max(offset, -1.0f), 1.0f);
  emit centerOffsetChanged();
}

float PositionHandler::centerOffset() { return mOffset; }

constexpr QPoint PositionHandler::computePosition(const QRect &screen,
                                                  const QRect &view,
                                                  const Layout &layout) {
  switch (layout.orientation()) {
  case Types::Orientation::Horizontal:
    return {calcAlign(screen.left(), screen.width(), view.left(), view.width(),
                      layout.alignment()),
            calcEdge(screen, view, layout.edge())};

  case Types::Orientation::Vertical:
    return {calcEdge(screen, view, layout.edge()),
            calcAlign(screen.top(), screen.height(), view.top(), view.height(),
                      layout.alignment())};
  }
  return {};
}

constexpr int PositionHandler::calcEdge(const QRect &screen, const QRect &view,
                                        Types::Edge edge) {
  switch (edge) {
  case Types::Edge::Top:
    return screen.top();
  case Types::Edge::Right:
    return screen.left() + screen.width() - view.width();
  case Types::Edge::Bottom:
    return screen.top() + screen.height() - view.height();
  case Types::Edge::Left:
    return screen.left();
  }
  return 0;
}

constexpr int PositionHandler::calcAlign(int screenStart, int screenWide,
                                         int viewStart, int viewWide,
                                         Types::Alignment align) {
  switch (align) {
  case Types::Alignment::Start:
    return -viewStart;
  case Types::Alignment::Center:
    return screenStart + int(screenWide - (viewStart * 2 + viewWide) +
                             mOffset * (screenWide - viewWide)) /
                             2;
  case Types::Alignment::End:
    return screenWide - (viewStart + viewWide);
  }
  return 0;
}

} // namespace Muelle
