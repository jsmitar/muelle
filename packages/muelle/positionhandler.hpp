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

#ifndef POSITIONHANDLER_HPP
#define POSITIONHANDLER_HPP

#include "docktypes.hpp"

#include <cmath>

#include <QObject>
#include <QPoint>
#include <QPropertyAnimation>
#include <QRect>
#include <QScreen>
#include <QSize>
#include <QWindow>

namespace Muelle {
class View;
class Layout;

class PositionHandler : public QObject {
  Q_OBJECT
  Q_PROPERTY(float centerOffset READ centerOffset WRITE setCenterOffset NOTIFY
                 centerOffsetChanged)

public:
  explicit PositionHandler(View *view);
  virtual ~PositionHandler();

  Q_INVOKABLE void update(int duration);

  float centerOffset();
  void setCenterOffset(float centerOffset);

signals:
  void centerOffsetChanged();

private:
  constexpr QPoint computePosition(const QRect &screen, const QRect &view,
                                   const Layout &layout);

  constexpr int calcEdge(const QRect &screen, const QRect &view,
                         Types::Edge edge);

  constexpr int calcAlign(int screenStart, int screenWide, int viewStart,
                          int viewWide, Types::Alignment align);

  View *mView;
  float mOffset{0};
  QPropertyAnimation *animation{nullptr};
};
} // namespace Muelle
#endif // POSITIONHANDLER_HPP
