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

#ifndef HELPERS_HPP
#define HELPERS_HPP

#include "dockconfig.hpp"

#include <QApplication>
#include <QBitmap>
#include <QDebug>
#include <QImage>
#include <QJSEngine>
#include <QJSValue>
#include <QMetaObject>
#include <QObject>
#include <QPainter>
#include <QPen>
#include <QPixmap>
#include <QPolygonF>
#include <QQmlContext>
#include <QQmlEngine>
#include <QRegion>
#include <QString>
#include <QThread>
#include <QTimer>
#include <QVariant>
#include <cmath>
#include <iostream>
#include <memory>

#ifdef __GNUG__
#include <cstdlib>
#include <cxxabi.h>
#include <memory>
static inline std::string demangle(const char *name) {
  int status = -4; // some arbitrary value to eliminate the compiler warning

  // enable c++11 by passing the flag -std=c++11 to g++
  std::unique_ptr<char, void (*)(void *)> res{
      abi::__cxa_demangle(name, NULL, NULL, &status), std::free};

  return (status == 0) ? res.get() : name;
}

#else
// does nothing if not g++
static inline std::string demangle(const char *name) { return name; }

#endif

template <typename V> constexpr auto min(const V &v) { return v; }

template <typename L, typename R, typename... Tail>
constexpr auto min(const L &l, const R &r, const Tail &... tail) {
  return min(l < r ? l : r, tail...);
}

template <typename V> constexpr auto max(const V &v) { return v; }

template <typename L, typename R, typename... Tail>
constexpr auto max(const L &l, const R &r, const Tail &... tail) {
  return max(l > r ? l : r, tail...);
}

template <typename B, typename V, typename T>
constexpr V clamp(const B &b, const V &v, const T &t) {
  if (b > t)
    return b;
  return max(b, min(v, t));
}

class Lambda {
public:
  Lambda() = default;
  ~Lambda() = default;

  Lambda(const Lambda &fn) { m_function = fn.m_function; }
  Lambda(Lambda &&fn) { m_function = fn.m_function; };

  template <class Fn, std::enable_if_t<
                          !std::is_same_v<std::decay_t<Fn>, Lambda>, int> = 0>
  Lambda(Fn &&function) {
    m_function.reset(new Callable(std::forward<Fn>(function)));
  }

  Lambda &operator=(Lambda &fn) {
    std::swap(*this, fn);
    return *this;
  };

  Lambda &operator=(const Lambda &fn) {
    Lambda temp(fn);
    std::swap(*this, temp);
    return *this;
  }

  friend void swap(Lambda &l, Lambda &r) {
    using std::swap;
    swap(l.m_function, r.m_function);
  }

  void operator()() {
    if (m_function) {
      m_function->operator()();
    }
  }

  struct Concept {
    virtual ~Concept(){};
    virtual void operator()() = 0;
  };

  template <class Fn> struct Callable : Concept {
    Callable(Fn &&function) : m_fn(std::forward<Fn>(function)) {}
    ~Callable() override {}

    void operator()() override { m_fn(); }
    Fn m_fn;
  };

private:
  std::shared_ptr<Concept> m_function;
};
Q_DECLARE_METATYPE(Lambda);

namespace Muelle {

class Extensions : public QObject {
  Q_OBJECT
public:
  explicit Extensions(QObject *parent = nullptr) : QObject(parent) {}
  virtual ~Extensions() override {}

  inline QVariant setTimeout(const QVariant &handler, int delay,
                             bool singleShot) const {
    auto value = handler.value<QJSValue>();

    if (!value.isCallable()) {
      qWarning() << "handler is not a value callable";
      return {QJSValue::SpecialValue::UndefinedValue};
    }

    QTimer *timer = new QTimer();
    timer->setSingleShot(singleShot);
    timer->callOnTimeout([value]() mutable { value.call(); });

    Lambda disconnect([timer]() mutable {
      if (timer) {
        if (timer->isActive()) {
          QMetaObject::invokeMethod(timer, "stop", Qt::QueuedConnection);
        }
        timer->deleteLater();
        timer = nullptr;
      }
    });
    if (singleShot)
      timer->callOnTimeout(disconnect);

    timer->start(delay + delay * 0.05);

    return QVariant::fromValue(disconnect);
  }

  Q_INVOKABLE QVariant setTimeout(const QVariant &handler, int delay) const {
    return setTimeout(handler, delay, true);
  }

  Q_INVOKABLE void clearTimeout(const QVariant &handler) const {
    if (handler.canConvert<Lambda>()) {
      auto disconnect = handler.value<Lambda>();
      disconnect();
    }
  }

  Q_INVOKABLE QVariant setInterval(const QVariant &handler, int delay) const {
    return setTimeout(handler, delay, false);
  }

  Q_INVOKABLE void clearInterval(const QVariant &handler) const {
    clearTimeout(handler);
  }
};

static void registerExtensions(QQmlEngine &engine) {
  static Muelle::Extensions ext;
  engine.rootContext()->setContextObject(&ext);
  engine.rootContext()->setContextProperty(
      QStringLiteral("__muelle_separator__"), MUELLE_SEPARATOR);
}

} // namespace Muelle

#endif // HELPERS_HPP
