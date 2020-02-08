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
#include <QPoint>
#include <QPointer>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQmlIncubator>
#include <QQuickItem>
#include <QQuickWindow>
#include <QRect>
#include <QSharedPointer>
#include <QSize>

namespace Muelle {
class View : public QQuickWindow {
  Q_OBJECT
  Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)
  Q_PROPERTY(QRect mask READ mask WRITE setMask NOTIFY maskChanged)
  Q_PROPERTY(QRect geometry READ geometry NOTIFY maskChanged)
  Q_PROPERTY(QRect panelGeometry READ panelGeometry NOTIFY panelGeometryChanged)

  Q_PROPERTY(QSize panelSize READ panelSize WRITE setPanelSize)
  Q_PROPERTY(QPoint panelPosition READ panelPosition WRITE setPanelPosition)
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

  bool compositing() const;
  void setMask(const QRect &region);
  Q_INVOKABLE void setOpacity(qreal level);

  const Layout &layout() const;

  QRect mask() const;

  QRect geometry() const;
  void setPosition(const QPoint &value);
  void setSize(const QSize &size);

  QRect absolutePanelGeometry() const;

  QPoint panelPosition() const;
  void setPanelPosition(const QPoint &value);
  QRect panelGeometry() const;

  QSize panelSize() const;
  void setPanelSize(const QSize &value);

  QPoint mousePosition() const;
  bool containsMouse() const;
  void setContainsMouse(bool containsMouse);

  Configuration *configuration() const;

signals:
  void compositingChanged();
  void maskChanged();

  void positionChanged();
  void sizeChanged();

  void panelSizeChanged();
  void panelPositionChanged();
  void panelGeometryChanged();

  void containsMouseChanged();
  void entered();
  void exited();

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
