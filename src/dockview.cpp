#include "dockview.hpp"
#include <xcb/xcb.h>
#include <xcb/xinput.h>

#include <KWindowEffects>
#include <KWindowSystem>

namespace Dock {
View::View(QSharedPointer<EnhancedQmlEngine> &engine)
    : QQuickView(engine.data(), nullptr), mBehavior(this),
      mPositionHandler(this), mEngine(engine) {
  setColor(Qt::transparent);
  setFlag(Qt::WindowStaysOnTopHint, true);
  setFlag(Qt::WindowDoesNotAcceptFocus, true);
  setFlag(Qt::WindowCloseButtonHint, false);
  setFlag(Qt::FramelessWindowHint, true);
  setResizeMode(QQuickView::SizeRootObjectToView);
  setTextRenderType(QQuickWindow::NativeTextRendering);
  setPersistentSceneGraph(false);
  setPersistentOpenGLContext(false);

  KWindowSystem::setType(winId(), NET::Dock);

  connect(this, &QQuickView::widthChanged, this, &View::sizeChanged);
  connect(this, &QQuickView::heightChanged, this, &View::sizeChanged);

  connect(mEngine.data(), &EnhancedQmlEngine::sourceChanged, this, &View::load);
  connect(mEngine.data(), &EnhancedQmlEngine::clearSource, [&] { setSource({}); });
}

View::~View() {}

void View::load() {
  connect(this, &View::statusChanged,
          [this](auto status) { init = status == QQuickView::Ready; });
  rootContext()->setContextProperty("$view", this);
  rootContext()->setContextProperty("$layout", &mLayout);
  rootContext()->setContextProperty("$behavior", &mBehavior);
  rootContext()->setContextProperty("$positioner", &mPositionHandler);
  clearBeforeRendering();
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

void View::setPanelGeometry(const QRect &value)
{
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
}

void View::setSize(const QSize &size) {
  setWidth(size.width());
  setHeight(size.height());
  mPositionHandler.update(0);
  emit sizeChanged();
}

QPoint View::mousePosition() const { return QCursor::pos(); }

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
