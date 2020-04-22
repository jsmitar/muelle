#include "windoweventfilter.hpp"
#include "dockview.hpp"

namespace Muelle {

WindowEventFilter::WindowEventFilter(QObject *parent) noexcept
    : QObject(parent), mCursorWatcher(new QTimer(this)) {
  mCursorWatcher->setInterval(250);
  mCursorWatcher->setSingleShot(false);
  mCursorWatcher->callOnTimeout([&, view = qobject_cast<View *>(parent)] {
    const auto containsMouse{
        view->absolutePanelGeometry().contains(QCursor::pos())};

    view->setContainsMouse(containsMouse);

    if (!containsMouse)
      mCursorWatcher->stop();
  });

  parent->installEventFilter(this);
}

bool WindowEventFilter::eventFilter(QObject *object, QEvent *event) {
  View *const view = qobject_cast<View *>(object);

  switch (event->type()) {
  case QEvent::Enter:
    view->setContainsMouse(true);
    break;
  case QEvent::Leave:
    view->setContainsMouse(false);
    break;
  case QEvent::DragEnter:
    view->setContainsMouse(true);
    // mCursorWatcher->start();
    break;
  case QEvent::DragLeave:
    view->setContainsMouse(false);
    // mCursorWatcher->stop();
    break;
  case QEvent::Drop:
    view->setContainsMouse(true);
    // mCursorWatcher->stop();
  default:;
  }

  return QObject::eventFilter(object, event);
}

} // namespace Muelle