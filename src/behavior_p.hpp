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

#include "behavior.hpp"
#include "docktypes.hpp"

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

namespace Dock {
class Behavior;
class BehaviorPrivate : public QObject {
  Q_GADGET
public:
  BehaviorPrivate(Behavior *q) : qPtr(q) { dodgeDebounce.setSingleShot(true); }

  virtual ~BehaviorPrivate() {
    for (auto c : connections)
      QObject::disconnect(c);
  }

public:
  void setBehavior(Types::Behavior value) {
    for (auto c : connections)
      QObject::disconnect(c);

    dodgeDebounce.stop();
    connections += dodgeDebounce.callOnTimeout([&] {
      dodgeValue = nextDodgeValue;
      qPtr->dodgeChanged();
    });

    switch (behavior = value) {
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
    connections += QConnect(kwindow(), KWindowShowingDesktopChanged, qPtr,
                            &Behavior::showingDesktop);
    nextDodgeValue = true;
    dodgeDebounce.start();
  }

  void enableDodgeAll() {
    connections +=
        QConnect(kwindow(), KWindowAdded, this, &BehaviorPrivate::dodgeWindow);

    connections += QConnect(kwindow(), KWindowRemoved, this,
                            &BehaviorPrivate::dodgeWindow);

    connections += QConnect(kwindow(), KWindowChanged, this,
                            &BehaviorPrivate::dodgeWindowProps);

    connections += QConnect(kwindow(), KWindowDesktopChanged, this,
                            &BehaviorPrivate::scanAllWindows);

    connections += QConnect(kwindow(), KWindowShowingDesktopChanged, qPtr,
                            &Behavior::showingDesktop);
    scanAllWindows();
  }

  void enableDodgeActive() {
    connections += QConnect(kwindow(), KWindowAdded, this,
                            &BehaviorPrivate::dodgeActiveWindow);

    connections += QConnect(kwindow(), KWindowRemoved, this,
                            &BehaviorPrivate::dodgeActiveWindow);

    connections += QConnect(kwindow(), KWindowChanged, this,
                            &BehaviorPrivate::dodgeWindowProps);

    connections += QConnect(kwindow(),
                            KWindowActiveChanged,
                            this,
                            &BehaviorPrivate::dodgeActiveWindow);

    connections += QConnect(kwindow(),
                            KWindowShowingDesktopChanged,
                            qPtr,
                            &Behavior::showingDesktop);

    dodgeActiveWindow(kwindow()->activeWindow());
  }

  void enableAlwaysVisible() {
    nextDodgeValue = false;
    dodgeDebounce.start();
    updateStruts();
  }

  void updateStruts() {

    kwindow()->setStrut(parentWId, geometry.left(), geometry.right() + 1,
                        geometry.top(), geometry.bottom() + 1);
  }

  void enable(bool value) { qPtr->blockSignals(!value); }

  void dodge(bool value) {
    if (value == nextDodgeValue)
      return;

    nextDodgeValue = value;
    dodgeDebounce.start(100);
  }

  void dodgeWindowProps(WId wid, NET::Properties props) {
    if ((props & propsFilter()) != 0) {
      behavior == Types::Behavior::DodgeActive ? dodgeActiveWindow(wid)
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
    if (kwindow()->activeWindow() == wid) {
      dodge(intersects({wid, propsFilter()}));
    }
  }

  bool intersects(const KWindowInfo &info, int desktop = -1) const {
    if (!info.valid() || info.win() == parentWId)
      return false;

    qDebug() << info.valid() << geometry << info.frameGeometry()
             << info.isOnCurrentDesktop() << info.onAllDesktops()
             << info.desktop() << info.isMinimized()
             << info.windowType(NET::DockMask | NET::MenuMask |
                                NET::DesktopMask)
             << info.frameGeometry().intersects(geometry);

    const bool validType = info.windowType(NET::NormalMask | NET::DialogMask |
                                           NET::ComboBoxMask) >= 0;

    const bool isOnCurrentDesktop =
        info.onAllDesktops() ||
        (desktop != -1 ? info.desktop() == desktop : info.isOnCurrentDesktop());

    return isOnCurrentDesktop && !info.isMinimized() && validType &&
           info.frameGeometry().intersects(geometry);
  }

  void scanAllWindows(int desktop = -1) {
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

  void setGeometry(const QRect &value) {
    geometry = value;
    switch (behavior) {
    case Types::Behavior::DodgeAll:
      scanAllWindows();
      break;
    case Types::Behavior::DodgeActive:
      dodgeActiveWindow(kwindow()->activeWindow());
      break;
    case Types::Behavior::AlwaysVisible:
      updateStruts();
      break;
    default:;
    }
  }

public:
  Behavior *qPtr{nullptr};
  QTimer dodgeDebounce;
  QVector<QMetaObject::Connection> connections;
  QRect geometry{};
  WId parentWId{0};
  Types::Behavior behavior{Types::Behavior::None};
  bool nextDodgeValue{false};
  bool dodgeValue{false};
};
} // namespace Dock

#endif // BEHAVIOR_P_HPP
