#ifndef DEBOUNCESIGNAL_HPP
#define DEBOUNCESIGNAL_HPP

#include <QMetaObject>
#include <QObject>
#include <QTimer>
#include <qglobal.h>

class DebounceSignal : public QObject {
  Q_OBJECT
public:
  explicit DebounceSignal(int timeout = 0, QObject *parent = nullptr);

  void setTimeout(int timeout);
  void start();

  template <typename Object, typename Signal>
  QMetaObject::Connection debounce(const Object *member, Signal &&signal) {
    return connect(member, std::forward<Signal>(signal), this,
                   &DebounceSignal::start);
  }

  template <typename Object, typename Functor>
  QMetaObject::Connection callOnTrigger(const Object *member, Functor &&slot) {
    return QObject::connect(this, &DebounceSignal::triggered, member,
                            std::forward<Functor>(slot));
  }

signals:
  void triggered();

private:
  QTimer *mTimer;
};

#endif // DEBOUNCESIGNAL_HPP
