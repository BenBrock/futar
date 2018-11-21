#pragma once

#include <utility>
#include <future>
#include <tuple>
#include <chrono>
#include <initializer_list>

namespace expl {

// Reference implementation of a simple future
template <typename Fn, typename... Args>
class future {
public:
  using return_type = decltype(std::declval<Fn>()(std::declval<Args...>()));

  future() = delete;
  future(const future&) = default;
  future(future&&) = default;

  future(Fn fn, Args... args) : fn_(fn), args_(std::make_tuple(args...)) {}

  return_type get() {
    return std::apply(std::move(fn_), std::move(args_));
  }

  template <class Rep, class Period>
  std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) const {
    return std::future_status::ready;
  }

private:
  Fn fn_;
  std::tuple<Args...> args_;
};

template <typename Fn, typename... Args>
future(Fn fn, Args... args) -> future<Fn, Args...>;

} // end expl
