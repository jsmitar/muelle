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

namespace Dock {
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
  constexpr QPoint computePosition(const QSize &screen, const QRect &view,
                                   const Layout &layout);

  constexpr int calcEdge(const QSize &screen, const QRect &view,
                         Types::Edge edge);

  constexpr int calcAlign(int screenWide, int viewStart, int viewWide,
                          Types::Alignment align);

  View *mView;
  float mOffset{0};
  QPropertyAnimation *animation{nullptr};
};
} // namespace Dock
#endif // POSITIONHANDLER_HPP
