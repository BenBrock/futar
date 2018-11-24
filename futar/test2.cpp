#include "future_wrapper.hpp"
#include "wrapper_future.hpp"
#include "detail/detail.hpp"
#include <iostream>

template <typename T>
T identity(T value) {
  return value;
}

template <typename T>
T add(T a, T b) {
  return a + b;
}

template <typename T,
          std::size_t tag = 0,
          typename = std::enable_if_t<futar::is_future<T>::value>>
auto wrap_future(T&& value) {
  return futar::future_wrapper<T>(std::move(value));
}

template <typename T,
          bool tag = 1,
          typename = std::enable_if_t<!futar::is_future<T>::value>>
auto wrap_future(T&& value) {
  return futar::wrapper_future<T>(std::move(value));
}

template <typename Fn, typename... Args>
auto apply(Fn fn, Args... args) {
  return fn(wrap_future<Args>(std::move(args)).get()...);
}

int main(int argc, char** argv) {
  auto result = apply(add<int>, std::async(identity<int>, 12), std::async(identity<int>, 13));
  std::cout << result2 << std::endl;
  return 0;
}
