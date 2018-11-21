#pragma once

#include <utility>

namespace expl {

template <typename FutureType, typename Fn, typename... Args>
class future_then {
public:
  using future_type = FutureType;
  using future_return_type = typename future_type::return_type;

  using return_type = decltype(
                               std::declval<Fn>()(
                                                  std::declval<future_return_type>(),
                                                  std::declval<Args...>()
                                                 )
                              );

  using size_type = size_t;

  future_then(Fn fn, future_type future, std::tuple<Args...> args)
              : fn_(fn), future_(future), args_(args) {}

  template <std::size_t... Is>
  return_type call_fn_impl_(std::index_sequence<Is...>) {
    return fn_(future_.get(), std::get<Is>(args_)...);
  }

  return_type get() {
    return call_fn_impl_(std::index_sequence_for<Args...>());
  }

private:
  Fn fn_;
  future_type future_;
  std::tuple<Args...> args_;
};

template <typename FutureType, typename Fn, typename... Args>
future_then(Fn fn, FutureType future, std::tuple<Args...> args) -> future_then<FutureType, Fn, Args...>;

} // end expl
