#pragma once

#include "detail/detail.hpp"
#include "future_wrapper.hpp"
#include "value_wrapper.hpp"
#include "future.hpp"

#include <utility>
#include <tuple>

#include <unistd.h>

namespace futar {

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
  return futar::value_wrapper<T>(std::move(value));
}

template <typename Fn, typename... Args>
struct future_result {
  using type = decltype(std::declval<Fn>()(wrap_future(std::declval<Args>()).get()...));
};

template <typename Fn, typename... Args>
using future_result_t = typename future_result <Fn, Args...>::type;

// Wrap a function as a future,
// arguments may be future types.
template <typename Fn, typename... Args>
class fn_wrapper final : public virtual future<future_result_t<Fn, Args...>> {
public:
  using return_type = future_result_t<Fn, Args...>;

  fn_wrapper(const Fn& fn, const Args&... args)
             : fn_(fn), args_(std::make_tuple(wrap_future<Args>(args)...)) {}

  fn_wrapper(Fn&& fn, Args&&... args)
             : fn_(std::move(fn)),
               args_(std::make_tuple(std::move(wrap_future(std::move(args)))...)) {}

  fn_wrapper(fn_wrapper&&) = default;

  ~fn_wrapper() override = default;

  template <std::size_t... Is>
  auto call_fn_impl_(std::index_sequence<Is...>) {
    return fn_(std::get<Is>(args_).get()...);
  }

  return_type get() override {
    return call_fn_impl_(std::index_sequence_for<Args...>());
  }

  fn_wrapper* move() override {
    return new fn_wrapper(std::move(*this));
  }

  template <class Rep, class Period>
  std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) const {
    auto begin = std::chrono::high_resolution_clock::now();
    auto end = begin;

    bool success = false;
    std::size_t sleep_time = 1;
    // OPTIMIZE: max sleep time in between checks 10ms
    std::size_t max_sleep = 10000;

    while (!(success = all_ready_()) && (end - begin) < timeout_duration) {
      usleep(sleep_time);
      sleep_time *= 2;
      sleep_time = std::min(sleep_time, max_sleep);
      end = std::chrono::high_resolution_clock::now();
    }

    if (success) {
      return std::future_status::ready;
    } else {
      return std::future_status::timeout;
    }
  }

  template <size_t I>
  bool check_futures_impl_() const {
    if (std::get<I>(args_).wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
      return false;
    }

    if constexpr(I+1 < sizeof...(Args)) {
      return check_futures_impl_<I+1>();
    }

    return true;
  }

  bool all_ready_() const {
    return check_futures_impl_<0>();
  }

private:
  Fn fn_;
  std::tuple<decltype(wrap_future(std::declval<Args>()))...> args_;
};

template <typename Fn, typename... Args>
fn_wrapper(Fn&& fn, Args&&... args) -> fn_wrapper<Fn, Args...>;

template <typename Fn, typename... Args>
fn_wrapper(const Fn& fn, const Args&... args) -> fn_wrapper<Fn, Args...>;

template <typename Fn, typename... Args>
auto call(Fn&& fn, Args&&... args) {
  return fn_wrapper(std::move(fn), std::move(args)...);
}

} // end futar
