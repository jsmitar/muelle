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
#include "helpers.hpp"
#include "layout.hpp"
#include "libs/enhancedqmlengine.hpp"
#include "positionhandler.hpp"
#include "viewshadows.hpp"
#include "windoweventfilter.hpp"

#include <KWindowEffects>
#include <KWindowSystem>
#include <QX11Info>
#include <xcb/shape.h>
#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <algorithm>
#include <memory>
#include <vector>

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
#include <QQuickView>
#include <QQuickWindow>
#include <QRect>
#include <QRegion>
#include <QScreen>
#include <QSharedPointer>
#include <QSize>
#include <QSurfaceFormat>
#include <QWindow>

namespace Muelle {
class View : public QQuickWindow {
  Q_OBJECT

  Q_PROPERTY(QScreen *screen READ screen WRITE setScreen NOTIFY screenChanged)

  Q_PROPERTY(bool compositing READ compositing NOTIFY compositingChanged)

  Q_PROPERTY(QPoint mousePosition READ mousePosition)
  Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)

  Q_PROPERTY(QRegion mask READ mask WRITE setMask NOTIFY maskChanged)
  Q_PROPERTY(QRegion inputMask READ inputMask WRITE setInputMask NOTIFY
                 inputMaskChanged)
  Q_PROPERTY(int frameExtents READ frameExtents WRITE setFrameExtents NOTIFY
                 frameExtentsChanged)

  Q_PROPERTY(
      QPoint position READ position WRITE setPosition NOTIFY positionChanged)
  Q_PROPERTY(QSize size READ size WRITE setSize NOTIFY sizeChanged)
  Q_PROPERTY(QRect geometry READ geometry NOTIFY geometryChanged)

  Q_PROPERTY(QRegion panelGeometry READ panelGeometry WRITE setPanelGeometry
                 NOTIFY panelGeometryChanged)
  Q_PROPERTY(QRegion absolutePanelGeometry READ absolutePanelGeometry NOTIFY
                 absolutePanelGeometryChanged)

  Q_PROPERTY(Muelle::Configuration *configuration READ configuration CONSTANT)

public:
  View(EnhancedQmlEngine *engine, KConfigGroup &config);
  ~View() override;

  void load();
  void continueLoad(QQmlComponent *component);
  void unload();

  void initX11();

  void setScreen(QScreen *screen);

  bool compositing() const;

  QPoint mousePosition() const;
  void setContainsMouse(bool containsMouse);
  bool containsMouse() const;

  void setMask(const QRegion &region);
  QRegion mask() const;
  void setInputMask(const QRegion &region);
  QRegion inputMask() const;
  void setFrameExtents(int value);
  int frameExtents() const;
  void updateFrameExtents();

  void setPosition(const QPoint &point);
  void setSize(const QSize &size);
  QRect geometry() const;

  void setPanelGeometry(const QRegion &rect);
  QRegion panelGeometry() const;
  QRegion absolutePanelGeometry() const;

  Configuration *configuration() const;
  const Layout &layout() const;

  Q_INVOKABLE void setOpacity(qreal level);

signals:
  void loaded();
  void release();

  void screenChanged();

  void compositingChanged();

  void containsMouseChanged();
  void entered();
  void exited();

  void maskChanged();
  void inputMaskChanged();
  void frameExtentsChanged();

  void positionChanged();
  void sizeChanged();
  void geometryChanged();

  void panelGeometryChanged();
  void absolutePanelGeometryChanged();

private:
  bool mContainsMouse{false};
  Layout mLayout;
  PositionHandler mPositionHandler;

  EnhancedQmlEngine *mEngine;
  QQmlContext *mContext;

  QRegion mPanelGeometry;
  QRegion mInputMask;
  int mFrameExtents;

  struct {
    xcb_atom_t gtk_frame_extents_atom;
    xcb_connection_t *connection;
    xcb_window_t wid;
  } x11;

  KConfigGroup mConfig;
  Configuration *mConfigMap;
};

} // namespace Muelle

#endif // DOCKVIEW_CPP
