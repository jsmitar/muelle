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
#ifndef BEHAVIOR_P_HPP
#define BEHAVIOR_P_HPP

#include "docktypes.hpp"
#include "dockview.hpp"
#include "libs/debouncesignal.hpp"
#include "panelbehavior.hpp"

#include <chrono>
#include <cstddef>
#include <functional>

#include <QMetaObject>
#include <QObject>
#include <QTimer>
#include <QVector>
#include <QtGlobal>

#include <KWindowInfo>
#include <KWindowSystem>
#include <NETWM>
#include <QDebug>
#include <netwm_def.h>

// KWindowSystem short alias
typedef KWindowSystem KWindow;
// inline decltype(auto) kwindow() { return KWindowSystem::self(); }

constexpr auto KWindowChanged{
    qOverload<WId, NET::Properties, NET::Properties2>(&KWindow::windowChanged)};

constexpr const auto kwindow{&KWindow::self};
constexpr auto KWindowAdded{&KWindow::windowAdded};
constexpr auto KWindowRemoved{&KWindow::windowRemoved};
constexpr auto KWindowActiveChanged{&KWindow::activeWindowChanged};
constexpr auto KWindowDesktopChanged{&KWindow::currentDesktopChanged};
constexpr auto KWindowShowingDesktopChanged{&KWindow::showingDesktopChanged};

constexpr NET::Properties NETProps = NET::ActiveWindow | NET::CloseWindow |
                                     NET::WMWindowType | NET::WMState |
                                     NET::WMDesktop | NET::WMFrameExtents |
                                     NET::WMGeometry | NET::DesktopGeometry;

struct Lock {
  bool value;
};

namespace Muelle {
class PanelBehavior;

namespace Private {

class PanelBehavior : public QObject {
  Q_OBJECT
public:
  PanelBehavior(Muelle::PanelBehavior *q)
      : q_ptr(q), connection(QX11Info::connection()) {
    dodgeDebounce.setSingleShot(true);
    dodgeDebounce.setInterval(100);
    dodgeDebounce.callOnTimeout([&] {
      dodgeValue = nextDodgeValue;
      emit q_ptr->dodgeChanged();
    });
  }

  virtual ~PanelBehavior() override {
    foreach (auto c, connections)
      QObject::disconnect(c);
  }

public:
  void updateBehavior() {
    foreach (auto c, connections)
      QObject::disconnect(c);

    clearStruts();

    const auto behavior = q_ptr->m_behavior;
    dodgeDebounce.stop();

    switch (behavior) {
    case Types::Behavior::AutoHide:
      enableAutoHide();
      break;
    case Types::Behavior::DodgeAll:
      enableDodgeAll();
      break;
    case Types::Behavior::DodgeActive:
      enableDodgeActive();
      break;
    case Types::Behavior::AlwaysVisible:
      enableAlwaysVisible();
      break;
    default:
      break;
    }
  }

  void enableAutoHide() {
    connections += connect(kwindow(), KWindowShowingDesktopChanged,
                           [this] { dodge(!kwindow()->showingDesktop()); });
    nextDodgeValue = true;
    dodgeDebounce.start(0);
  }

  void enableDodgeAll() {
    connections += connect(kwindow(), KWindowAdded, this,
                           &Private::PanelBehavior::dodgeWindow);

    connections += connect(kwindow(), KWindowRemoved, this,
                           &Private::PanelBehavior::dodgeWindow);

    connections += connect(kwindow(), KWindowChanged, this,
                           &Private::PanelBehavior::dodgeWindowProps);

    connections += connect(kwindow(), KWindowDesktopChanged, this,
                           &Private::PanelBehavior::scanAllWindows);

    connections += connect(kwindow(), KWindowShowingDesktopChanged, this,
                           &Private::PanelBehavior::scanAllWindows);

    connections +=
        connect(q_ptr->m_view, &Muelle::View::absolutePanelGeometryChanged,
                this, &Private::PanelBehavior::updateDodge);
    scanAllWindows();
  }

  void enableDodgeActive() {
    connections += connect(kwindow(), KWindowAdded, this,
                           &Private::PanelBehavior::dodgeActiveWindow);

    connections += connect(kwindow(), KWindowRemoved, this,
                           &Private::PanelBehavior::dodgeActiveWindow);

    connections += connect(kwindow(), KWindowChanged, this,
                           &Private::PanelBehavior::dodgeWindowProps);

    connections += connect(kwindow(), KWindowActiveChanged, this,
                           &Private::PanelBehavior::dodgeActiveWindow);

    connections += connect(kwindow(), KWindowShowingDesktopChanged, this,
                           &Private::PanelBehavior::dodgeActiveWindow);

    connections +=
        connect(q_ptr->m_view, &Muelle::View::absolutePanelGeometryChanged,
                this, &Private::PanelBehavior::updateDodge);

    dodgeActiveWindow();
  }

  void enableAlwaysVisible() {
    nextDodgeValue = false;
    dodgeDebounce.start(0);
  }

  void dodge(bool value) {
    nextDodgeValue = value;
    dodgeDebounce.start();
  }

  void dodgeWindowProps(WId wid, NET::Properties props) {
    if ((props & NETProps) != 0) {
      q_ptr->m_behavior == Types::Behavior::DodgeActive ? dodgeActiveWindow()
                                                        : dodgeWindow(wid);
    }
  }

  void dodgeWindow(WId wid) {
    if (intersects({wid, NETProps})) {
      dodge(true);
    } else {
      scanAllWindows();
    }
  }

  void dodgeActiveWindow() {
    if (kwindow()->showingDesktop()) {
      return dodge(false);
    }

    auto active = kwindow()->activeWindow();

    foreach (auto wid, kwindow()->windows()) {
      const KWindowInfo info{wid, NETProps};

      if (wid == active || info.hasState(NET::FullScreen)) {
        if (intersects(info, -1))
          return dodge(true);
      }
    }
    dodge(false);
  }

  bool intersects(const KWindowInfo &info, int desktop = -1) const {
    if (!info.valid() || info.win() == q_ptr->m_view->winId())
      return false;

    const auto winTypes = info.windowType(
        NET::NormalMask | NET::DialogMask | NET::ComboBoxMask | NET::DockMask |
        NET::NotificationMask | NET::CriticalNotificationMask);

    const auto validType = (winTypes & (NET::DockMask | NET::NotificationMask |
                                        NET::CriticalNotificationMask)) == 0;

    if (!validType)
      return false;

    auto frameGeometry{info.frameGeometry()};

    NETWinInfo net{connection,
                   static_cast<xcb_window_t>(info.win()),
                   {},
                   {},
                   NET::WM2GTKFrameExtents};
    const auto extents{net.gtkFrameExtents()};

    if (extents.top || extents.right || extents.bottom || extents.left) {
      frameGeometry = frameGeometry.marginsAdded(
          {-extents.left, -extents.top, -extents.right, -extents.bottom});
    }
    frameGeometry = frameGeometry.marginsAdded({-1, -1, -1, -1});

    const bool isOnCurrentDesktop =
        info.onAllDesktops() ||
        (desktop != -1 ? info.desktop() == desktop : info.isOnCurrentDesktop());

    return isOnCurrentDesktop && !info.isMinimized() &&
           q_ptr->m_view->absolutePanelGeometry().intersects(frameGeometry);
  }

  void scanAllWindows(int desktop = -1) {
    if (kwindow()->showingDesktop())
      return dodge(false);

    desktop == -1 ? kwindow()->currentDesktop() : desktop;

    foreach (auto wid, kwindow()->windows()) {
      if (intersects({wid, NETProps}, desktop)) {
        return dodge(true);
      }
    }
    dodge(false);
  }

  constexpr NET::Properties2 props2Filter() { return {}; }

  void updateDodge() {
    using Behavior = Types::Behavior;
    if (auto behavior = q_ptr->m_behavior; behavior == Behavior::DodgeAll) {
      scanAllWindows();
    } else if (behavior == Behavior::DodgeActive) {
      dodgeActiveWindow();
    }
  }

  void updateStruts() {
    if (q_ptr->m_behavior != Types::Behavior::AlwaysVisible)
      return;

    if (auto screen_ = q_ptr->m_view->screen(); screen_) {
      const auto panel = q_ptr->m_view->absolutePanelGeometry().boundingRect();
      const auto wid = q_ptr->m_view->winId();
      const auto screen = screen_->geometry();
      using Edge = Types::Edge;

      switch (q_ptr->m_view->layout().edge()) {
      case Edge::Top:
        kwindow()->setExtendedStrut(wid, 0, 0, 0, 0, 0, 0, panel.height(), 0,
                                    screen.width(), 0, 0, 0);
        break;
      case Edge::Right:
        kwindow()->setExtendedStrut(wid, 0, 0, 0, panel.width(), 0,
                                    screen.height(), 0, 0, 0, 0, 0, 0);
        break;
      case Edge::Bottom:
        kwindow()->setExtendedStrut(wid, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                    panel.height(), 0, screen.width());
        break;
      case Edge::Left:
        kwindow()->setExtendedStrut(wid, panel.width(), 0, screen.height(), 0,
                                    0, 0, 0, 0, 0, 0, 0, 0);
        break;
      }
    }
  }

  void clearStruts() {
    kwindow()->setExtendedStrut(q_ptr->m_view->winId(), 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0, 0);
  }

public:
  Muelle::PanelBehavior *q_ptr{nullptr};
  QTimer dodgeDebounce;
  QVector<QMetaObject::Connection> connections;
  xcb_connection_t *connection;
  bool nextDodgeValue{false};
  bool dodgeValue{false};
};
} // namespace Private
} // namespace Muelle

#endif // BEHAVIOR_P_HPP
