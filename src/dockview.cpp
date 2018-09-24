#include "dockview.hpp"

namespace Dock {
View::View() {}

View::~View() {}

void View::load() {
  connect(this, &View::statusChanged,
          [this](auto status) { this->init = status == QQuickView::Ready; });
  engine()->rootContext()->setContextProperty("view", this);
  engine()->rootContext()->setContextProperty("layout", &mLayout);
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

bool View::event(QEvent *ev) {
  if (!init) {
    return QQuickView::event(ev);
  }

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
