#pragma once

#include <tuple>
#include <utility>
#include <chrono>

#include <futar/detail/detail.hpp>

namespace futar {

template <typename FutureType, typename Fn, typename... Args>
class future_then {
public:
  using future_type = FutureType;
  using future_return_type = decltype(std::declval<future_type>().get());

  using return_type = decltype(
                               std::declval<Fn>()(
                                                  std::declval<future_return_type>(),
                                                  std::declval<Args...>()
                                                 )
                              );

  future_then() = delete;
  future_then(const future_then&) = delete;
  future_then(future_then&&) = default;

  future_then(Fn fn, future_type&& future, Args... args)
              : fn_(fn), future_(std::move(future)), args_(std::make_tuple(args...)) {}

  template <std::size_t... Is>
  return_type call_fn_impl_(std::index_sequence<Is...>) {
    return fn_(std::forward<decay_future<future_type>::type>(future_.get()),
               std::forward<Args>(std::get<Is>(args_))...);
  }

  return_type get() {
    return call_fn_impl_(std::index_sequence_for<Args...>());
  }

  template <class Rep, class Period>
  std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) const {
    return future_.wait_for(timeout_duration);
  }

private:
  Fn fn_;
  future_type future_;
  std::tuple<Args...> args_;
};

template <typename FutureType, typename Fn, typename... Args>
future_then(Fn fn, FutureType&& future, Args... args) -> future_then<FutureType, Fn, Args...>;

} // end futar
