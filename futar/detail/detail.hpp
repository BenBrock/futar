#pragma once

#include <utility>
#include <type_traits>
#include <future>

namespace futar {

template <typename T, typename... Args>
struct first_type {
  using type = T;
};

template <typename T, typename... Args>
using first_type_t = typename first_type<T, Args...>::type;

template <typename... Ts>
struct parameter_pack {
  template <template <typename...> typename T>
  struct apply {
    using type = T<Ts...>;
  };
};

template <typename T>
struct is_future {

  template <typename U>
  static constexpr decltype(std::declval<U>().get(), bool())
  test_get(int) {
    return true;
  }

  template <typename U>
  static constexpr bool test_get(...) {
    return false;
  }

  template <typename U>
  static constexpr
    std::enable_if_t<std::is_same<decltype(std::declval<U>().wait_for(std::chrono::seconds(0))),
                                  std::future_status>::value, bool>
  test_wait_for(int) {
    return true;
  }

  template <typename U>
  static constexpr bool test_wait_for(...) {
    return false;
  }

  static constexpr bool value = test_get<T>(int()) && test_wait_for<T>(int());
};

template <typename T, typename Enabler = void>
struct decay_future {
  using type = T;
};

template <typename T>
struct decay_future<T, std::enable_if_t<is_future<T>::value>> {
  using type = decltype(std::declval<T>().get());
};

template <typename T>
using decay_future_t = typename decay_future<T>::type;

template <typename T, typename Enabler = void>
struct decay_chain {
  using type = T;
};

template <typename T>
struct decay_chain<T, std::enable_if_t<is_future<T>::value>> {
  using type = typename decay_chain<decltype(std::declval<T>().get())>::type;
};

template <typename T>
using decay_chain_t = typename decay_chain<T>::type;

} // end futar
