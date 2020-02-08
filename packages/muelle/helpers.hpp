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

#include <QApplication>
#include <QDebug>
#include <QJSEngine>
#include <QJSValue>
#include <QMetaObject>
#include <QObject>
#include <QQmlContext>
#include <QQmlEngine>
#include <QSharedPointer>
#include <QThread>
#include <QTimer>
#include <QVariant>
#include <iostream>
#include <memory>

#ifdef __GNUG__
#include <cstdlib>
#include <memory>
#include <cxxabi.h>
static inline std::string demangle(const char* name) {
  int status = -4; // some arbitrary value to eliminate the compiler warning

  // enable c++11 by passing the flag -std=c++11 to g++
  std::unique_ptr<char, void(*)(void*)> res {
      abi::__cxa_demangle(name, NULL, NULL, &status),
      std::free
  };

  return (status==0) ? res.get() : name ;
}

#else
// does nothing if not g++
static inline std::string demangle(const char* name) {
    return name;
}

#endif

class Lambda {
public:
  Lambda() = default;
  ~Lambda() = default;

  Lambda(const Lambda& fn) {
    m_function = fn.m_function;
  }
  Lambda(Lambda && fn) {
    m_function = fn.m_function;
  };

  template<class Fn, std::enable_if_t<!std::is_same_v<std::decay_t<Fn>, Lambda>, int> = 0>
  Lambda(Fn&& function) {
    m_function.reset(new Callable(std::forward<Fn>(function)));
  }

  Lambda &operator=(Lambda& fn) {
    std::swap(*this, fn);
    return *this;
  };

  Lambda &operator=(const Lambda& fn) {
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
    virtual ~Concept() {};
    virtual void operator()() = 0;
  };

  template<class Fn>
  struct Callable : Concept {
    Callable(Fn &&function): m_fn(std::forward<Fn>(function)) {}
    ~Callable() override {}

    void operator()() override {
      m_fn();
    }
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
  explicit Extensions(QObject * parent = nullptr): QObject(parent) {}
  virtual ~Extensions() override {}

  Q_INVOKABLE void clearTimeout(const QVariant &handler) {
    if (handler.canConvert<Lambda>()) {
      auto disconnect = handler.value<Lambda>();
      disconnect();
    }
  }

  Q_INVOKABLE QVariant setTimeout(const QVariant &handler, int delay) {
    auto value = handler.value<QJSValue>();

    if (!value.isCallable()) {
      qWarning() << "handler is not a value callable";
      return {QJSValue::SpecialValue::UndefinedValue};
    }

    QTimer * timer = new QTimer();

    Lambda disconnect([timer] () mutable {
      if (timer) {
        if (timer->isActive()) {
          QMetaObject::invokeMethod(timer, "stop", Qt::QueuedConnection);
        }
        timer->deleteLater();
        timer = nullptr;
      }
    });

    timer->callOnTimeout([value] () mutable { value.call(); });
    timer->callOnTimeout(disconnect);
    timer->setSingleShot(true);
    timer->start(delay + delay * 0.05);

    return QVariant::fromValue(disconnect);
  }

  static void registerExtensions(QQmlEngine &engine) {
    static Extensions ext;
    engine.rootContext()->setContextObject(&ext);
  }
};

}


#endif // HELPERS_HPP

