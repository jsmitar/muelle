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

namespace Muelle {
class PressureDetector : public QObject,
                         public QAbstractNativeEventFilter,
                         public QQmlParserStatus {
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(double threshold MEMBER mThreshold NOTIFY thresholdChanged)
  Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
  Q_PROPERTY(Muelle::View *view MEMBER view)
  Q_PROPERTY(
      Muelle::Types::Edge edge READ edge WRITE setEdge NOTIFY edgeChanged)

public:
  PressureDetector(QObject *parent = nullptr);

  virtual ~PressureDetector() override;

  virtual void classBegin() override;
  virtual void componentComplete() override;

  Muelle::Types::Edge edge() const;
  void setEdge(Muelle::Types::Edge edge);

  void setEnabled(bool enabled);
  bool enabled() const;

  void setView(Muelle::View *view);

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

    std::string toString() const {
      return "(" + std::to_string(x1) + "," + std::to_string(y1) + ") " +
             std::to_string(x2 - x1) + "x" + std::to_string(y2 - y1);
    }
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
  Muelle::Types::Edge mEdge{Types::Edge::Bottom};
  bool mEnabled = false;
  bool mBarrierSupport = false;
  bool mReached = false;
};

} // namespace Muelle
inline void qmlRegisterPanelBehavior() {
  qmlRegisterType<Muelle::PressureDetector>("org.muelle.extra", 1, 0,
                                            "PressureDetector");
}

#endif // PRESSUREDETECTOR_HPP
