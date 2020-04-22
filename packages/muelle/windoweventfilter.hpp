#ifndef WINDOWEVENTFILTER_HPP
#define WINDOWEVENTFILTER_HPP

#include <QCursor>
#include <QEvent>
#include <QObject>
#include <QTimer>

namespace Muelle {
class View;
class WindowEventFilter : public QObject {
  Q_OBJECT

public:
  WindowEventFilter(QObject *parent) noexcept;

protected:
  bool eventFilter(QObject *obj, QEvent *event) override;

private:
  // Sometimes the DragLeave event is not sent to the QWindow
  // when DragEnter is reported I need to watch the cursor pos until that
  // the cursor is outside of the View
  QTimer *mCursorWatcher;
};
} // namespace Muelle

#endif // WINDOWEVENTFILTER_HPP