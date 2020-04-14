#include "debouncesignal.hpp"

DebounceSignal::DebounceSignal(int timeout, QObject *parent)
    : QObject(parent), mTimer(new QTimer(this)) {

  mTimer->setSingleShot(true);
  mTimer->setInterval(timeout);
  mTimer->callOnTimeout(this, &DebounceSignal::triggered);
}

void DebounceSignal::setTimeout(int timeout) { mTimer->setInterval(timeout); }

void DebounceSignal::start() {
  if (mThrotle) {
    if (!mTimer->isActive()) {
      mTimer->start();
    }
  } else {
    mTimer->start();
  }
}

void DebounceSignal::setThrotle(bool enable) { mThrotle = enable; }