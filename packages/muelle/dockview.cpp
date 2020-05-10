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

namespace Muelle {
View::View(EnhancedQmlEngine *engine, KConfigGroup &config)
    : mPositionHandler(this), mEngine(engine),
      mContext(new QQmlContext(mEngine, this)), mConfig(&config, ""),
      mConfigMap(new Configuration(this, config)) {

  setColor(Qt::transparent);
  setFlag(Qt::WindowStaysOnTopHint);
  setFlag(Qt::WindowDoesNotAcceptFocus);
  setFlag(Qt::WindowCloseButtonHint);
  setFlag(Qt::FramelessWindowHint);
  setFlag(Qt::X11BypassWindowManagerHint);
  setFlag(Qt::NoDropShadowWindowHint);

  setTextRenderType(QQuickWindow::NativeTextRendering);
  setPersistentSceneGraph(true);
  setPersistentOpenGLContext(true);
  setClearBeforeRendering(true);

  KWindowSystem::setType(winId(), NET::Dock);
  KWindowSystem::setOnAllDesktops(winId(), true);
  KWindowSystem::setOnActivities(winId(), {"0"});
  KWindowSystem::setState(winId(), NET::SkipPager | NET::SkipSwitcher |
                                       NET::StaysOnTop | NET::Sticky);

  connect(this, &QQuickWindow::widthChanged, this, &View::sizeChanged);
  connect(this, &QQuickWindow::heightChanged, this, &View::sizeChanged);
  connect(this, &View::panelPositionChanged, this, &View::panelGeometryChanged);
  connect(this, &View::panelSizeChanged, this, &View::panelGeometryChanged);
  connect(KWindowSystem::self(), &KWindowSystem::compositingChanged, this,
          &View::compositingChanged);

  connect(this, &QQuickWindow::screenChanged, this, &View::screenChanged);

  new WindowEventFilter(this);
}

View::~View() {
  mEngine->collectGarbage();
  mEngine->clearComponentCache();
  mEngine->trimComponentCache();
}

void View::load() {
  qInfo() << "Window-Id:" << winId();

  mContext->setContextProperty("$view", this);
  mContext->setContextProperty("$layout", &mLayout);
  mContext->setContextProperty("$positioner", &mPositionHandler);
  mContext->setContextProperty("$configuration", mConfigMap);

  auto component = new QQmlComponent(mEngine, QUrl("qrc:/shell/main.qml"),
                                     QQmlComponent::Asynchronous);

  if (component->isLoading()) {
    connect(component, &QQmlComponent::statusChanged,
            [=] { continueLoad(component); });
  } else {
    continueLoad(component);
  }
}

void View::continueLoad(QQmlComponent *component) {
  if (component->isReady()) {
    auto root = qobject_cast<QQuickItem *>(component->create(mContext));
    root->setObjectName(QStringLiteral("__root__"));
    root->setParent(contentItem());
    root->setParentItem(contentItem());

    show();
    setOpacity(0.01);
  } else if (component->isError()) {
    qWarning() << component->errors();
  }
}

void View::unload() {
  emit release();

  auto root = contentItem()->findChild<QQuickItem *>(
      QStringLiteral("__root__"), Qt::FindDirectChildrenOnly);

  root->setVisible(false);
  root->setEnabled(false);

  auto timer = new QTimer(this);
  timer->setSingleShot(true);
  timer->callOnTimeout(this, [&, root] {
    if (root) {
      root->setParentItem(nullptr);
      root->setParent(nullptr);
      root->deleteLater();
      deleteLater();
    } else {
      qDebug() << "unload: root(null) this should never ocurr";
    }
  });
  timer->start(100);
}

bool View::containsMouse() const {
  return absolutePanelGeometry().contains(QCursor::pos());
}

void View::setContainsMouse(bool value) {
  if (value != mContainsMouse) {
    (mContainsMouse = value) ? emit entered() : emit exited();
    emit containsMouseChanged();
  }
}

QRect View::mask() const { return QQuickWindow::mask().boundingRect(); }

QRect View::geometry() const { return {position(), size()}; }

QRect View::panelGeometry() const { return mPanelGeometry; }

QSize View::panelSize() const { return mPanelGeometry.size(); }

void View::setPanelSize(const QSize &value) {
  mPanelGeometry.setSize(value);
  emit panelSizeChanged();
  mPositionHandler.update(200);
}

QRect View::absolutePanelGeometry() const {
  return {position() + mPanelGeometry.topLeft(), mPanelGeometry.size()};
}

QPoint View::panelPosition() const { return mPanelGeometry.topLeft(); }

void View::setPanelPosition(const QPoint &value) {
  mPanelGeometry.moveTo(value);
  emit panelPositionChanged();
  mPositionHandler.update(200);
}

void View::setMask(const QRect &rect) {
  QQuickWindow::setMask(rect);
  emit maskChanged();
}

void View::setPosition(const QPoint &value) {
  QQuickWindow::setPosition(value);
  emit positionChanged();
  emit panelGeometryChanged();
}

void View::setSize(const QSize &size) {
  setWidth(size.width());
  setHeight(size.height());

  contentItem()->setSize(size);
  mPositionHandler.update(0);
  emit sizeChanged();
}

QPoint View::mousePosition() const { return QCursor::pos(); }

bool View::compositing() const { return KWindowSystem::compositingActive(); }

Configuration *View::configuration() const { return mConfigMap; }

// void View::enableGlow() {
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
//}

void View::setOpacity(qreal level) { QQuickWindow::setOpacity(level); }

const Layout &View::layout() const { return mLayout; }

} // namespace Muelle
