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
#ifndef BEHAVIOR_H
#define BEHAVIOR_H

#include "docktypes.hpp"
#include "dockview.hpp"

#include <QObject>
#include <QQmlParserStatus>
#include <QRect>
#include <QtGui/qwindowdefs.h>

namespace Muelle {
class View;
namespace Private {
class PanelBehavior;
};

class PanelBehavior : public QObject, public QQmlParserStatus {
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(Muelle::Types::Behavior behavior READ behavior WRITE setBehavior
                 NOTIFY behaviorChanged)
  Q_PROPERTY(bool dodge READ dodge NOTIFY dodgeChanged)
  Q_PROPERTY(Muelle::View *view MEMBER view)

public:
  explicit PanelBehavior(QObject *parent = nullptr);
  virtual ~PanelBehavior() override;

  void classBegin() override;
  void componentComplete() override;

  Types::Behavior behavior() const;
  void setBehavior(Types::Behavior behavior);

  Q_INVOKABLE void updateStruts();

  bool dodge() const;

signals:
  void behaviorChanged();
  void dodgeChanged();

private:
  Private::PanelBehavior *dPtr;
  Types::Behavior mBehavior{Types::Behavior::None};
  View *view;

  friend class Private::PanelBehavior;
};

} // namespace Muelle
inline void qmlRegisterPressureDetector() {
  qmlRegisterType<Muelle::PanelBehavior>("org.muelle.extra", 1, 0,
                                         "PanelBehavior");
}
#endif // BEHAVIOR_H
