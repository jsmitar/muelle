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

#include "pressuredetector.hpp"
#include <QGuiApplication>
#include <X11/extensions/Xfixes.h>

namespace Dock {
PressureDetector::PressureDetector(QObject *parent) : QObject(parent) {}

PressureDetector::~PressureDetector() {
  removePointerBarrier();
  qGuiApp->removeNativeEventFilter(this);
}

void PressureDetector::classBegin() { initPointerBarrier(); }

void PressureDetector::componentComplete() {
  if (mBarrierSupport) {
    mPressureTimer.setSingleShot(true);
    mPressureTimer.callOnTimeout([&] { mPressure = 0; });

    connect(view, &View::panelGeometryChanged, this,
            &PressureDetector::geometryChanged);

    auto debouncer = new DebounceSignal(250, this);

    debouncer->debounce(this, &PressureDetector::edgeChanged);
    debouncer->debounce(this, &PressureDetector::geometryChanged);
    debouncer->debounce(this, &PressureDetector::enabledChanged);

    debouncer->callOnTrigger(this, &PressureDetector::updateBarrier);
    qGuiApp->installNativeEventFilter(this);
  }
}

Types::Edge PressureDetector::edge() const { return mEdge; }

void PressureDetector::setEdge(Types::Edge edge) {
  if (mEdge == edge)
    return;

  mEdge = edge;
  emit edgeChanged();
}

void PressureDetector::setEnabled(bool enabled) {
  if (mEnabled == enabled)
    return;

  mEnabled = enabled;

  emit enabledChanged();
}

bool PressureDetector::enabled() const { return mEnabled; }

void PressureDetector::setView(View *view) { this->view = view; }

bool PressureDetector::nativeEventFilter(const QByteArray &eventType,
                                         void *message, long *) {
  if (mReached)
    return false;

  if (mBarrier && eventType == "xcb_generic_event_t") {
    auto *ev = static_cast<xcb_generic_event_t *>(message);

    switch (ev->response_type) {
    case XCB_GE_GENERIC: {
      switch (static_cast<xcb_ge_event_t *>(message)->event_type) {
      case XCB_INPUT_BARRIER_HIT: {
        auto event = static_cast<xcb_input_barrier_hit_event_t *>(message);
        if (event->barrier != mBarrier)
          return false;

        auto slide = 0.0;
        auto distance = 0.0;

        using Edge = Dock::Types::Edge;
        switch (mEdge) {
        case Edge::Top:
        case Edge::Bottom:
          slide = std::abs(event->dx.integral);
          distance = std::abs(event->dy.integral);
          break;

        case Edge::Left:
        case Edge::Right:
          slide = std::abs(event->dy.integral);
          distance = std::abs(event->dx.integral);
          break;
        }

        if (slide < distance) {
          distance = std::min(15.0, distance);
          mPressure += distance;
          mPressureTimer.start(500);
        }

        if (mPressure >= mThreshold) {
          mReached = true;
          mPressure = 0;
          mPressureTimer.stop();
          emit thresholdReached();

          xcb_input_barrier_release_pointer_info_t info;
          info.barrier = event->barrier;
          info.eventid = event->eventid;
          info.deviceid = event->deviceid;
          xcb_input_xi_barrier_release_pointer(QX11Info::connection(), 1,
                                               &info);
        }
        return true;
      }
      case XCB_INPUT_BARRIER_LEAVE:
        auto event = static_cast<xcb_input_barrier_leave_event_t *>(message);
        if (event->barrier != mBarrier)
          return false;

        mPressureTimer.start(500);
        return true;
      }
      break;
    }
    }
  }
  return false;
}

void PressureDetector::initPointerBarrier() {
  auto conn = QX11Info::connection();

  if (auto query = xcb_get_extension_data(conn, &xcb_xfixes_id);
      !query->present)
    return;

  if (auto query = xcb_get_extension_data(conn, &xcb_input_id); !query->present)
    return;

  mBarrierSupport = true;
}

void PressureDetector::removePointerBarrier() {
  if (mBarrier > 0) {
    auto conn = QX11Info::connection();
    auto cookie = xcb_xfixes_delete_pointer_barrier_checked(conn, mBarrier);

    if (auto error = xcb_request_check(conn, cookie); error) {
      qWarning() << "fail to delete barrier" << error->response_type;
    } else {
      mBarrier = 0;
    }
  }
}

void PressureDetector::updateBarrier() {
  removePointerBarrier();

  if (mEnabled && view) {
    auto conn = QX11Info::connection();
    auto winId = static_cast<uint>(view->winId());
    auto [direction, rect] = barrier();

    mBarrier = xcb_generate_id(conn);
    auto cookie = xcb_xfixes_create_pointer_barrier_checked(
        conn, mBarrier, winId, rect.x1, rect.y1, rect.x2, rect.y2, direction, 0,
        nullptr);

    /* Enable pointer barrier events */ {
      struct {
        xcb_input_event_mask_t head;
        uint32_t mask;
      } mask;

      mask.mask = XCB_INPUT_XI_EVENT_MASK_BARRIER_HIT |
                  XCB_INPUT_XI_EVENT_MASK_BARRIER_LEAVE;
      mask.head.deviceid = XCB_INPUT_DEVICE_ALL;
      mask.head.mask_len = (sizeof(mask.mask) / sizeof(uint32_t));

      xcb_input_xi_select_events(conn, winId, 1, &mask.head);
    }

    if (auto error = xcb_request_check(conn, cookie); error) {
      qWarning() << "fail to create pointer barrier" << error->error_code;
      mBarrier = 0;
      return;
    }
    mReached = false;
  }
}

std::tuple<xcb_xfixes_barrier_directions_t, PressureDetector::Rect>
PressureDetector::barrier() const {
  using Edge = Dock::Types::Edge;

  auto xcb_rect = [](int x1, int y1, int x2, int y2) -> PressureDetector::Rect {
    return {static_cast<uint16_t>(x1), static_cast<uint16_t>(y1),
            static_cast<uint16_t>(x2), static_cast<uint16_t>(y2)};
  };

  auto r = view->absolutePanelGeometry();
  switch (mEdge) {
  case Edge::Top:
    return {XCB_XFIXES_BARRIER_DIRECTIONS_POSITIVE_Y,
            xcb_rect(r.left(), r.top(), r.right() + 1, r.top())};
  case Edge::Right:
    return {XCB_XFIXES_BARRIER_DIRECTIONS_NEGATIVE_X,
            xcb_rect(r.right() + 1, r.top(), r.right() + 1, r.bottom() + 1)};
  case Edge::Bottom:
    return {XCB_XFIXES_BARRIER_DIRECTIONS_NEGATIVE_Y,
            xcb_rect(r.left(), r.bottom() + 1, r.right(), r.bottom() + 1)};
  case Edge::Left:
    return {XCB_XFIXES_BARRIER_DIRECTIONS_POSITIVE_X,
            xcb_rect(r.left(), r.top(), r.left(), r.bottom() + 1)};
  }

  return {};
}

} // namespace Dock
