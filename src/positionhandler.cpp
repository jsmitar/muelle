#include "positionhandler.hpp"
#include "dockview.hpp"
#include <QPropertyAnimation>

namespace Dock {
PositionHandler::PositionHandler(View *view)
  : mView(view)
  , animation(new QPropertyAnimation(view))
{
  animation->setTargetObject(mView);
  animation->setPropertyName("position");
}

PositionHandler::~PositionHandler() {}

void PositionHandler::update(int duration)
{
  if (auto screen = mView->screen(); screen) {
    const auto newPosition = computePosition(screen->geometry().size(),
                                             mView->panelGeometry(),
                                             mView->layout());
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

void PositionHandler::setCenterOffset(float offset) {
  mOffset = std::min(std::max(offset, -1.0f), 1.0f);
  emit centerOffsetChanged();
}

float PositionHandler::centerOffset() { return mOffset; }

constexpr QPoint PositionHandler::computePosition(const QSize &screen,
                                                  const QRect &view,
                                                  const Layout &layout) {
  switch (layout.orientation()) {
  case Types::Orientation::Horizontal:
    return {
        calcAlign(screen.width(), view.x(), view.width(), layout.alignment()),
        calcEdge(screen, view, layout.edge())};

  case Types::Orientation::Vertical:
    return {calcEdge(screen, view, layout.edge()),
            calcAlign(screen.height(), view.y(), view.height(),
                      layout.alignment())};
  }
  return {};
}

constexpr int PositionHandler::calcEdge(const QSize &screen, const QRect &view,
                                        Types::Edge edge) {
  switch (edge) {
  case Types::Edge::Top:
    return 0;
  case Types::Edge::Right:
    return screen.width() - view.width();
  case Types::Edge::Bottom:
    return screen.height() - view.height();
  case Types::Edge::Left:
    return 0;
  }
  return 0;
}

constexpr int PositionHandler::calcAlign(int screenWide, int viewStart,
                                         int viewWide, Types::Alignment align) {
  switch (align) {
  case Types::Alignment::Start:
    return -viewStart;
  case Types::Alignment::Center:
    return int(screenWide - (viewStart * 2 + viewWide) +
               mOffset * (screenWide - viewWide)) /
           2;
  case Types::Alignment::End:
    return screenWide - (viewStart + viewWide);
  }
  return 0;
}

} // namespace Dock
