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

#ifndef PRESSUREDETECTOR_HPP
#define PRESSUREDETECTOR_HPP

#include "docktypes.hpp"
#include "dockview.hpp"
#include "libs/debouncesignal.hpp"

#include <QAbstractNativeEventFilter>
#include <QDebug>
#include <QObject>
#include <QQmlEngine>
#include <QQmlParserStatus>
#include <QRect>
#include <QScreen>
#include <QWindow>
#include <QtX11Extras/QX11Info>

#include <tuple>

#include <xcb/xcb.h>
#include <xcb/xcb_util.h>
#include <xcb/xinput.h>

namespace Dock {
class PressureDetector : public QObject,
                         public QAbstractNativeEventFilter,
                         public QQmlParserStatus {
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(double threshold MEMBER mThreshold NOTIFY thresholdChanged)
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(Dock::View *view WRITE setView)
  Q_PROPERTY(Dock::Types::Edge edge READ edge WRITE setEdge NOTIFY edgeChanged)

public:
  PressureDetector(QObject *parent = nullptr);

  virtual ~PressureDetector() override;

  virtual void classBegin() override;
  virtual void componentComplete() override;

  Dock::Types::Edge edge() const;
  void setEdge(Dock::Types::Edge edge);

  void setEnabled(bool enabled);
  bool enabled() const;

  void setView(Dock::View *view);

  bool nativeEventFilter(const QByteArray &eventType, void *message,
                         long *result) override;

  Q_INVOKABLE void updateBarrier();

private:
  void initPointerBarrier();
  void removePointerBarrier();

  struct Rect {
    uint16_t x1;
    uint16_t y1;
    uint16_t x2;
    uint16_t y2;
  };
  std::tuple<xcb_xfixes_barrier_directions_t, Rect> barrier() const;

signals:
  void thresholdReached();
  void thresholdChanged();
  void enabledChanged();
  void edgeChanged();
  void geometryChanged();

private:
  View *view;
  double mThreshold = 60;
  double mPressure = 0;
  QTimer mPressureTimer;
  xcb_xfixes_barrier_t mBarrier = 0;
  Dock::Types::Edge mEdge;
  bool mEnabled = false;
  bool mBarrierSupport = false;
  bool mReached = false;
};

} // namespace Dock
inline void qmlRegisterPressureDetector() {
  qmlRegisterType<Dock::PressureDetector>("org.muelle.extra", 1, 0,
                                          "PressureDetector");
}

#endif // PRESSUREDETECTOR_HPP
