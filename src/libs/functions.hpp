#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <any>
#include <chrono>
#include <functional>
#include <memory>
#include <ratio>
#include <string>
#include <tuple>
#include <type_traits>

#include <QTimer>

#define qs QStringLiteral

using namespace std;

template <class Functor, class Arg> class Debounce final {
  std::unique_ptr<QTimer> timer;
  std::any lastArg;

public:
  Debounce(chrono::milliseconds ms, Functor &&f)
      : timer(make_unique<QTimer>()) {
    timer->setInterval(ms);
    timer->setSingleShot(true);
    timer->callOnTimeout(
        [&f, lastArg = &lastArg]() { f(std::any_cast<Arg>(*lastArg)); });
  }

  Debounce(const Debounce &) = delete;

  Debounce(Debounce &&other) { swap(*this, other); }

  Debounce &operator=(const Debounce &) = delete;

  Debounce &operator=(Debounce &&other) {
    swap(*this, other);
    return *this;
  }

  void operator()(Arg arg) {
    if (timer->isActive()) {
      lastArg = arg;
      timer->start();
    };
  }

  void swap(Debounce &first, Debounce &second) {
    std::swap(first.timer, second.timer);
    std::swap(first.lastArg, second.lastArg);
  }
};

template <class Functor, class Arg>
auto _make_debounce(chrono::milliseconds wait, Functor &&f, void (*)(Arg)) {
  return std::move(Debounce<Functor, Arg>(wait, f));
}

template <class Fn, class Functor>
auto make_debounce(chrono::milliseconds wait, Functor &&f) {
  return _make_debounce(wait, f, static_cast<Fn *>(nullptr));
}

// Taked from
// https://riptutorial.com/cplusplus/example/8508/recursive-lambdas
template <class Fn> class y_combinator_impl {
  Fn fn;

public:
  template <class T>
  explicit y_combinator_impl(T &&fn) : fn(std::forward<T>(fn)) {}

  template <class... Args> decltype(auto) operator()(Args &&... args) {
    return fn(std::ref(*this), args...);
  }
};

template <class Fn> decltype(auto) y_combinator(Fn &&function) {
  return y_combinator_impl<std::decay_t<Fn>>(std::forward<Fn>(function));
}

#endif // FUNCTIONS_HPP
