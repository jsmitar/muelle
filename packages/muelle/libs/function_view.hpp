#ifndef FUNCTION_VIEW_HPP
#define FUNCTION_VIEW_HPP

#pragma once

#include <functional>
#include <type_traits>

template <typename TSignature> class function_view;

template <typename TReturn, typename... TArgs>
class function_view<TReturn(TArgs...)> final {
private:
  using signature_type = TReturn(void *, TArgs...);

  signature_type *_erased_fn;
  void *_ptr;

public:
  template <typename T, typename = std::enable_if_t<
                            std::is_invocable_v<T &(TArgs...)> &&
                            !std::is_same<std::decay_t<T>, function_view>{}>>
  function_view(T &&x) noexcept : _ptr{static_cast<void *>(std::addressof(x))} {
    _erased_fn = [](void *ptr, TArgs... xs) -> TReturn {
      return (*reinterpret_cast<std::add_pointer_t<T>>(ptr))(
          std::forward<TArgs>(xs)...);
    };
  }

  decltype(auto) operator()(TArgs... xs) const
      noexcept(noexcept(_erased_fn(_ptr, std::forward<TArgs>(xs)...))) {
    return _erased_fn(_ptr, std::forward<TArgs>(xs)...);
  }
};

#endif // FUNCTION_VIEW_HPP
