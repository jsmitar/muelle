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

#include <QJSValue>
#include <QObject>
#include <QQmlParserStatus>
#include <QRect>
#include <QVariant>
#include <QtGui/qwindowdefs.h>

namespace Muelle {
class View;
namespace Private {
class PanelBehavior;
};

class PanelBehavior : public QObject, public QQmlParserStatus {
  Q_OBJECT
  Q_INTERFACES(QQmlParserStatus)
  Q_PROPERTY(Muelle::View *view READ view WRITE setView)
  Q_PROPERTY(Muelle::Types::Behavior behavior READ behavior WRITE setBehavior
                 NOTIFY behaviorChanged)
  Q_PROPERTY(bool dodge READ dodge NOTIFY dodgeChanged)

public:
  explicit PanelBehavior(QObject *parent = nullptr) noexcept;
  virtual ~PanelBehavior() override;

  void classBegin() override;
  void componentComplete() override;

  void setView(View *view);
  View *view() const;

  Types::Behavior behavior() const noexcept;
  void setBehavior(Types::Behavior behavior) noexcept;

  bool dodge() const noexcept;

  Q_INVOKABLE void updateStruts() noexcept;
  Q_INVOKABLE void scanWindows() noexcept;

signals:
  void behaviorChanged();
  void dodgeChanged();

private:
  Private::PanelBehavior *d_ptr;
  Types::Behavior m_behavior{Types::Behavior::None};
  View *m_view;

  friend class Private::PanelBehavior;
};

} // namespace Muelle
inline void qmlRegisterPressureDetector() {
  qmlRegisterType<Muelle::PanelBehavior>("org.muelle.extra", 1, 0,
                                         "PanelBehavior");
}
#endif // BEHAVIOR_H
