#pragma once

#include <utility>
#include <type_traits>
#include <future>

namespace futar {

template <typename T>
struct decay_future {
  using type = decltype(std::declval<T>().get());
};

template <typename T, typename... Args>
struct first_type {
  using type = T;
};

template <typename T, typename... Args>
using first_type_t = typename first_type<T, Args...>::type;

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

} // end futar
