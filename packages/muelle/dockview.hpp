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

#ifndef DOCKVIEW_HPP
#define DOCKVIEW_HPP

#include "configuration.hpp"
#include "layout.hpp"
#include "libs/enhancedqmlengine.hpp"
#include "positionhandler.hpp"

#include <memory>

#include <KConfig>
#include <KConfigGroup>
#include <QDebug>
#include <QEvent>
#include <QObject>
#include <QPointer>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlIncubator>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRect>
#include <QSharedPointer>

namespace Muelle {
class View : public QQuickWindow {
  Q_OBJECT
  Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)
  Q_PROPERTY(QRect mask READ mask WRITE setMask NOTIFY maskChanged)
  Q_PROPERTY(QRect geometry READ geometry NOTIFY maskChanged)
  Q_PROPERTY(QRect panelGeometry READ panelGeometry WRITE setPanelGeometry
                 NOTIFY panelGeometryChanged)
  Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)
  Q_PROPERTY(
      QPoint position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(QPoint mousePosition READ mousePosition)
  Q_PROPERTY(bool compositing READ compositing NOTIFY compositingChanged)
  Q_PROPERTY(Muelle::Configuration *configuration READ configuration CONSTANT)

public:
  View(EnhancedQmlEngine *engine, KConfigGroup &config);
  ~View() override;

  void init();
  void load();

  bool containsMouse() const;
  void setContainsMouse(bool containsMouse);
  void setMask(const QRect &region);
  const Layout &layout() const;
  QRect mask() const;
  QRect geometry() const;
  QRect panelGeometry() const;
  QRect absolutePanelGeometry() const;
  void setPanelGeometry(const QRect &value);
  void setPosition(const QPoint &value);
  void setSize(const QSize &size);
  QPoint mousePosition() const;
  bool compositing() const;
  Configuration *configuration() const;

  Q_INVOKABLE void saveConfiguration();
  Q_INVOKABLE void enableGlow();
  Q_INVOKABLE void setOpacity(qreal level);

signals:
  void containsMouseChanged();
  void entered();
  void exited();
  void maskChanged();
  void positionChanged();
  void compositingChanged();
  void sizeChanged();
  void panelGeometryChanged();
  void release();

protected:
  bool event(QEvent *ev) override;

private:
  bool mContainsMouse = false;

  Layout mLayout;
  PositionHandler mPositionHandler;

  EnhancedQmlEngine *mEngine;
  QQmlContext *mContext;

  QRect mPanelGeometry;
  KConfigGroup mConfig;
  Configuration *mConfigMap;
};

} // namespace Muelle

#endif // DOCKVIEW_CPP
