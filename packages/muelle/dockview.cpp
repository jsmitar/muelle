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

namespace Muelle {
View::View(EnhancedQmlEngine *engine, KConfigGroup &config)
    : mPositionHandler(this), mEngine(engine),
      mContext(new QQmlContext(mEngine, this)), mConfig(&config, ""),
      mConfigMap(new Configuration(this, config)) {

  initX11();

  setColor(Qt::transparent);
  setFlags(Qt::WindowDoesNotAcceptFocus | Qt::WindowStaysOnTopHint |
           Qt::WindowCloseButtonHint | Qt::FramelessWindowHint |
           Qt::NoDropShadowWindowHint);

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
  connect(this, &View::positionChanged, this, &View::panelGeometryChanged);
  connect(this, &View::sizeChanged, this, &View::panelGeometryChanged);
  connect(this, &View::panelGeometryChanged, this,
          &View::absolutePanelGeometryChanged);

  connect(KWindowSystem::self(), &KWindowSystem::compositingChanged, this,
          &View::compositingChanged);
  connect(this, &QQuickWindow::screenChanged, this, &View::screenChanged);
  connect(&mLayout, &Layout::edgeChanged, this, &View::updateFrameExtents);

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

    setOpacity(0.01);
    show();
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

void View::initX11() {
  x11.connection = QX11Info::connection();
  x11.wid = winId();

  const auto prop = QByteArrayLiteral("_GTK_FRAME_EXTENTS");

  auto atomCookie = xcb_intern_atom_unchecked(
      x11.connection, false, static_cast<uint16_t>(prop.length()),
      prop.constData());

  const std::unique_ptr<xcb_intern_atom_reply_t> atom{
      xcb_intern_atom_reply(x11.connection, atomCookie, nullptr)};

  x11.gtk_frame_extents_atom = atom->atom;
}

void View::setScreen(QScreen *screen) { QQuickWindow::setScreen(screen); }

bool View::compositing() const { return KWindowSystem::compositingActive(); }

QPoint View::mousePosition() const { return QCursor::pos(); }

void View::setContainsMouse(bool value) {
  if (value != mContainsMouse) {
    (mContainsMouse = value) ? emit entered() : emit exited();
    emit containsMouseChanged();
  }
}

bool View::containsMouse() const { return mContainsMouse; }

void View::setMask(const QRegion &region) {
  if (!compositing()) {
    QQuickWindow::setMask(region);
    emit maskChanged();
  }
}

QRegion View::mask() const { return QQuickWindow::mask().boundingRect(); }

void View::setInputMask(const QRegion &region) {
  mInputMask = region;

  if (!region.isEmpty()) {
    std::vector<xcb_rectangle_t> xcb_region(region.rectCount());
    std::transform(region.cbegin(), region.cend(), xcb_region.begin(),
                   [](const auto &rect) -> xcb_rectangle_t {
                     return {.x = qMax<int16_t>(-1, rect.x()),
                             .y = qMax<int16_t>(-1, rect.y()),
                             .width = qMin<uint16_t>(USHRT_MAX, rect.width()),
                             .height =
                                 qMin<uint16_t>(USHRT_MAX, rect.height())};
                   });

    xcb_shape_rectangles(x11.connection, XCB_SHAPE_SO_SET, XCB_SHAPE_SK_INPUT,
                         XCB_CLIP_ORDERING_UNSORTED, x11.wid, 0, 0,
                         xcb_region.size(), xcb_region.data());

  } else {
    xcb_shape_mask(x11.connection, XCB_SHAPE_SO_INTERSECT, XCB_SHAPE_SK_INPUT,
                   x11.wid, 0, 0, XCB_PIXMAP_NONE);
  }

  updateFrameExtents();

  emit inputMaskChanged();
}

QRegion View::inputMask() const { return mInputMask; }

void View::setFrameExtents(int value) {
  if (value != qMax(0, mFrameExtents)) {
    mFrameExtents = value;
    updateFrameExtents();
    emit frameExtentsChanged();
  }
}

int View::frameExtents() const { return mFrameExtents; }

void View::updateFrameExtents() {
  // order: left, right, top, bottom
  uint32_t data[4]{0, 0, 0, 0};

  using Edge = Types::Edge;
  switch (mLayout.edge()) {
  case Edge::Top:
    data[3] = mFrameExtents; // bottom frame
    break;
  case Edge::Right:
    data[0] = mFrameExtents; // left frame
    break;
  case Edge::Bottom:
    data[2] = mFrameExtents; // top frame
    break;
  case Edge::Left:
    data[1] = mFrameExtents; // right frame
    break;
  }

  xcb_change_property(x11.connection, XCB_PROP_MODE_REPLACE,
                      static_cast<uint>(winId()), x11.gtk_frame_extents_atom,
                      XCB_ATOM_CARDINAL, 32, 4,
                      static_cast<const void *>(data));
}

void View::setPosition(const QPoint &value) {
  QQuickWindow::setPosition(value);
  emit positionChanged();
}

void View::setSize(const QSize &size) {
  setWidth(size.width());
  setHeight(size.height());
  contentItem()->setSize(size);

  mPositionHandler.update(0);
  emit sizeChanged();
}

QRect View::geometry() const { return {position(), size()}; }

void View::setPanelGeometry(const QRegion &region) {
  const QRect bound{{}, size()};

  if (auto panelGeometry = region & bound; mPanelGeometry != panelGeometry) {
    mPanelGeometry = panelGeometry;
    emit panelGeometryChanged();
  }
}

QRegion View::panelGeometry() const { return mPanelGeometry; }

QRegion View::absolutePanelGeometry() const {
  return mPanelGeometry.translated(position());
}

Configuration *View::configuration() const { return mConfigMap; }

const Layout &View::layout() const { return mLayout; }

void View::setOpacity(qreal level) { QQuickWindow::setOpacity(level); }
} // namespace Muelle
