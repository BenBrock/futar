#pragma once

#include <utility>
#include <future>
#include <tuple>
#include <initializer_list>

namespace expl {

// Reference implementation of a simple future
template <typename Fn, typename... Args>
class future {
public:

  using future_type = decltype(std::declval<Fn>()(std::declval<Args...>()));

  future(Fn fn, std::tuple<Args...> args) : fn_(fn), args_(args) {}

  future_type get() {
    return std::apply(fn_, std::move(args_));
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
future(Fn fn, std::tuple<Args...> args) -> future<Fn, Args...>;

}