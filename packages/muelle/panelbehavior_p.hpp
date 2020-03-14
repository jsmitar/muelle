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

#ifndef QConnect
#define QConnect QObject::connect
#endif

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

struct Lock {
  bool value;
};

namespace Muelle {
class PanelBehavior;

namespace Private {

class PanelBehavior : public QObject {
  Q_OBJECT
public:
  PanelBehavior(Muelle::PanelBehavior *q) : qPtr(q) {
    dodgeDebounce.setSingleShot(true);
    dodgeDebounce.setInterval(100);
    dodgeDebounce.callOnTimeout([&] {
      dodgeValue = nextDodgeValue;
      emit qPtr->dodgeChanged();
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

    const auto behavior = qPtr->mBehavior;
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
    connections += QConnect(kwindow(), KWindowShowingDesktopChanged,
                            [this] { dodge(!kwindow()->showingDesktop()); });
    nextDodgeValue = true;
    dodgeDebounce.start(0);
  }

  void enableDodgeAll() {
    connections += QConnect(kwindow(), KWindowAdded, this,
                            &Private::PanelBehavior::dodgeWindow);

    connections += QConnect(kwindow(), KWindowRemoved, this,
                            &Private::PanelBehavior::dodgeWindow);

    connections += QConnect(kwindow(), KWindowChanged, this,
                            &Private::PanelBehavior::dodgeWindowProps);

    connections += QConnect(kwindow(), KWindowDesktopChanged, this,
                            &Private::PanelBehavior::scanAllWindows);

    connections += QConnect(kwindow(), KWindowShowingDesktopChanged, this,
                            &Private::PanelBehavior::scanAllWindows);

    connections += QConnect(qPtr, &Muelle::PanelBehavior::regionChanged, this,
                            &Private::PanelBehavior::updateDodge);
    scanAllWindows();
  }

  void enableDodgeActive() {
    connections += QConnect(kwindow(), KWindowAdded, this,
                            &Private::PanelBehavior::dodgeActiveWindow);

    connections += QConnect(kwindow(), KWindowRemoved, this,
                            &Private::PanelBehavior::dodgeActiveWindow);

    connections += QConnect(kwindow(), KWindowChanged, this,
                            &Private::PanelBehavior::dodgeWindowProps);

    connections += QConnect(kwindow(), KWindowActiveChanged, this,
                            &Private::PanelBehavior::dodgeActiveWindow);

    connections += QConnect(kwindow(), KWindowShowingDesktopChanged, this,
                            &Private::PanelBehavior::dodgeActiveWindow);

    connections += QConnect(qPtr, &Muelle::PanelBehavior::regionChanged, this,
                            &Private::PanelBehavior::updateDodge);

    dodgeActiveWindow(kwindow()->activeWindow());
  }

  void enableAlwaysVisible() {
    nextDodgeValue = false;
    dodgeDebounce.start(0);
  }

  void dodge(bool value) {
    if (value == nextDodgeValue)
      return;

    nextDodgeValue = value;
    dodgeDebounce.start();
  }

  void dodgeWindowProps(WId wid, NET::Properties props) {
    if ((props & propsFilter()) != 0) {
      qPtr->mBehavior == Types::Behavior::DodgeActive ? dodgeActiveWindow(wid)
                                                      : dodgeWindow(wid);
    }
  }

  void dodgeWindow(WId wid) {
    if (intersects({wid, propsFilter()})) {
      dodge(true);
    } else {
      scanAllWindows();
    }
  }

  void dodgeActiveWindow(WId wid) {
    if (kwindow()->showingDesktop()) {
      dodge(false);
    } else if (kwindow()->activeWindow() == wid) {
      dodge(intersects({wid, propsFilter()}));
    }
  }

  bool intersects(const KWindowInfo &info, int desktop = -1) const {
    if (!info.valid() || info.win() == qPtr->view->winId())
      return false;

    const auto panel = qPtr->view->absolutePanelGeometry();
    //    qDebug() << "valid:" << info.valid() << "panel:" << panel
    //             << "frameRect:" << info.frameGeometry()
    //             << "isOnCurrentDesktop:" << info.isOnCurrentDesktop()
    //             << "allDesktops:" << info.onAllDesktops()
    //             << "desktop:" << info.desktop()
    //             << "isMinimized:" << info.isMinimized() << "winType:"
    //             << info.windowType(NET::DockMask | NET::MenuMask |
    //                                NET::DesktopMask)
    //             << "intersects:" << info.frameGeometry().intersects(panel);

    const bool validType = info.windowType(NET::NormalMask | NET::DialogMask |
                                           NET::ComboBoxMask) >= 0;

    const bool isOnCurrentDesktop =
        info.onAllDesktops() ||
        (desktop != -1 ? info.desktop() == desktop : info.isOnCurrentDesktop());

    return isOnCurrentDesktop && !info.isMinimized() && validType &&
           region.intersects(info.frameGeometry());
  }

  void scanAllWindows(int desktop = -1) {
    if (kwindow()->showingDesktop())
      return dodge(false);

    desktop == -1 ? kwindow()->currentDesktop() : desktop;

    foreach (auto wid, kwindow()->windows()) {
      if (intersects({wid, propsFilter()}, desktop)) {
        return dodge(true);
      }
    }
    dodge(false);
  }

  constexpr NET::Properties propsFilter() {
    return NET::ActiveWindow | NET::CloseWindow | NET::WMWindowType |
           NET::WMState | NET::WMDesktop | NET::WMGeometry |
           NET::WMFrameExtents | NET::DesktopGeometry;
  }

  void updateDodge() {
    using Behavior = Types::Behavior;
    if (auto behavior = qPtr->mBehavior; behavior == Behavior::DodgeAll) {
      scanAllWindows();
    } else if (behavior == Behavior::DodgeActive) {
      dodgeActiveWindow(kwindow()->activeWindow());
    }
  }

  void updateStruts() {
    if (qPtr->mBehavior != Types::Behavior::AlwaysVisible)
      return;

    if (auto screen_ = qPtr->view->screen(); screen_) {
      const auto panel = region.boundingRect();
      const auto wid = qPtr->view->winId();
      const auto screen = screen_->geometry();
      using Edge = Types::Edge;

      switch (qPtr->view->layout().edge()) {
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
    kwindow()->setExtendedStrut(qPtr->view->winId(), 0, 0, 0, 0, 0, 0, 0, 0, 0,
                                0, 0, 0);
  }

public:
  Muelle::PanelBehavior *qPtr{nullptr};
  QRegion region;
  QTimer dodgeDebounce;
  QVector<QMetaObject::Connection> connections;
  bool nextDodgeValue{false};
  bool dodgeValue{false};
  uint8_t pad[6];
};
} // namespace Private
} // namespace Muelle

#endif // BEHAVIOR_P_HPP
