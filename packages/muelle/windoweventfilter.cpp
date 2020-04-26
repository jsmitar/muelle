#include "windoweventfilter.hpp"
#include "dockview.hpp"

namespace Muelle {

WindowEventFilter::WindowEventFilter(QObject *parent) noexcept
    : QObject(parent), mCursorWatcher(new QTimer(this)) {
  mCursorWatcher->setInterval(400);
  mCursorWatcher->setSingleShot(false);
  mCursorWatcher->callOnTimeout([&, view = qobject_cast<View *>(parent)] {
    if (view->containsMouse()) {
      emit view->entered();
    } else {
      emit view->exited();
    }
  });

  parent->installEventFilter(this);
}

bool WindowEventFilter::eventFilter(QObject *object, QEvent *event) {
  View *const view = qobject_cast<View *>(object);

  switch (event->type()) {
  case QEvent::Enter:
  case QEvent::DragEnter:
  case QEvent::Drop:
    view->setContainsMouse(true);
    break;
  case QEvent::Leave:
  case QEvent::DragLeave:
    view->setContainsMouse(false);
  default:;
  }

  return QObject::eventFilter(object, event);
}

} // namespace Muelle