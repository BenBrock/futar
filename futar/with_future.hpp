#pragma once

#include <utility>
#include <tuple>
#include <chrono>

namespace futar {

template <typename Fn, typename... Args>
class with_future {
public:

  using return_type = decltype(
                               std::declval<Fn>()(
                                 std::declval<Args>().get()...
                               )
                              );

  with_future() = delete;
  with_future(const with_future&) = default;
  with_future(with_future&&) = default;

  with_future(Fn&& fn, Args&&... args) : fn_(std::move(fn)), args_(std::make_tuple(std::move(args)...)) {}

  template <std::size_t... Is>
  return_type call_fn_impl_(std::index_sequence<Is...>) {
    // TODO: add std::forward?
    return fn_(std::get<Is>(args_).get()...);
  }

  return_type get() {
    return call_fn_impl_(std::index_sequence_for<Args...>());
  }

  template <class Rep, class Period>
  std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) {
    auto begin = std::chrono::high_resolution_clock::now();

    auto end = begin;

    bool success = false;
    std::size_t sleep_time = 1;
    // max sleep time in between checks 10ms
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
  bool check_futures_impl_() {
    if (std::get<I>(args_).wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
      return false;
    }

    if constexpr(I+1 < sizeof...(Args)) {
      return check_futures_impl_<I+1>();
    }

    return true;
  }

  bool all_ready_() {
    return check_futures_impl_<0>();
  }

private:
  Fn fn_;
  std::tuple<Args...> args_;
};

template <typename Fn, typename... Args>
with_future(Fn&& fn, Args&&... args) -> with_future<Fn, Args...>;

} // end futar
