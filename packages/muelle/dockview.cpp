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

#include "dockview.hpp"
#include <memory>
#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <KWindowEffects>
#include <KWindowSystem>
#include <QX11Info>

namespace Dock {
View::View(QSharedPointer<EnhancedQmlEngine> &engine)
    : QQuickView(engine.data(), nullptr), mPositionHandler(this),
      mEngine(engine) {
  setColor(Qt::transparent);
  setFlag(Qt::WindowStaysOnTopHint, true);
  setFlag(Qt::WindowDoesNotAcceptFocus, true);
  setFlag(Qt::WindowCloseButtonHint, false);
  setFlag(Qt::FramelessWindowHint, true);

  setResizeMode(QQuickView::SizeRootObjectToView);
  setTextRenderType(QQuickWindow::NativeTextRendering);
  setPersistentSceneGraph(true);
  setPersistentOpenGLContext(true);
  setClearBeforeRendering(true);

  KWindowSystem::setType(winId(), NET::Dock);
  KWindowSystem::setOnAllDesktops(winId(), true);
  KWindowSystem::setOnActivities(winId(), {"0"});

  connect(this, &QQuickView::widthChanged, this, &View::sizeChanged);
  connect(this, &QQuickView::heightChanged, this, &View::sizeChanged);

  connect(mEngine.data(), &EnhancedQmlEngine::sourceChanged, this, &View::load);
  connect(mEngine.data(), &EnhancedQmlEngine::clearSource, [this] {
    hide();
    setSource({});
    rootContext()->setContextProperty("$view", nullptr);
    rootContext()->setContextProperty("$layout", nullptr);
    rootContext()->setContextProperty("$positioner", nullptr);
    releaseResources();
  });

  qInfo() << "Window id:" << winId();
}

View::~View() {}

void View::load() {
  rootContext()->setContextProperty("$view", this);
  rootContext()->setContextProperty("$layout", &mLayout);
  rootContext()->setContextProperty("$positioner", &mPositionHandler);
  setSource({"qrc:/main.qml"});
  show();
}

bool View::containsMouse() const { return mContainsMouse; }

void View::setContainsMouse(bool containsMouse) {
  if (mContainsMouse != containsMouse) {
    mContainsMouse = containsMouse;
    emit containsMouseChanged();
    containsMouse ? emit entered() : emit exited();
  }
}

QRect View::mask() const { return QQuickView::mask().boundingRect(); }

QRect View::geometry() const { return {position(), size()}; }

QRect View::panelGeometry() const { return mPanelGeometry; }

QRect View::absolutePanelGeometry() const {
  return {position() + mPanelGeometry.topLeft(), mPanelGeometry.size()};
}

void View::setPanelGeometry(const QRect &value) {
  mPanelGeometry = value;
  emit panelGeometryChanged();
  mPositionHandler.update(200);
}

void View::setMask(const QRect &rect) {
  QQuickView::setMask(rect);
  emit maskChanged();
}

void View::setPosition(const QPoint &value) {
  QQuickView::setPosition(value);
  emit positionChanged();
  emit panelGeometryChanged();
}

void View::setSize(const QSize &size) {
  setWidth(size.width());
  setHeight(size.height());
  mPositionHandler.update(0);
  emit sizeChanged();
}

QPoint View::mousePosition() const { return QCursor::pos(); }

void View::enableGlow() {
  //  auto c = QX11Info::connection();
  //  const auto effectName = QByteArrayLiteral("_KDE_NET_WM_SCREEN_EDGE_SHOW");
  //  auto atomCookie = xcb_intern_atom_unchecked(
  //      c, false, static_cast<uint16_t>(effectName.length()),
  //      effectName.constData());

  //  const std::unique_ptr<xcb_intern_atom_reply_t> atom{
  //      xcb_intern_atom_reply(c, atomCookie, nullptr)};

  //  uint32_t value = 0;
  //  using Edge = Types::Edge;
  //  switch (mLayout.edge()) {
  //  case Edge::Top:
  //    value = 0;
  //    break;
  //  case Edge::Right:
  //    value = 1;
  //    break;
  //  case Edge::Bottom:
  //    value = 2;
  //    break;
  //  case Edge::Left:
  //    value = 3;
  //    break;
  //  }

  //  xcb_change_property(c, XCB_PROP_MODE_REPLACE, static_cast<uint>(winId()),
  //                      atom->atom, XCB_ATOM_CARDINAL, 32, 1, &value);
}

void View::setOpacity(qreal level) { QQuickView::setOpacity(level); }

const Layout &View::layout() const { return mLayout; }

bool View::event(QEvent *ev) {
  switch (ev->type()) {
  case QEvent::Enter:
    setContainsMouse(true);
    break;
  case QEvent::Leave:
    setContainsMouse(false);
    break;
  default:;
  }
  return QQuickView::event(ev);
}

} // namespace Dock
