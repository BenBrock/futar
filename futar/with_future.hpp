#pragma once

#include <utility>
#include <tuple>

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

  with_future(const Fn& fn, const Args&... args) : fn_(fn), args_(std::make_tuple(args...)) {}
  with_future(Fn&& fn, Args&&... args) : fn_(std::move(fn)), args_(std::make_tuple(std::move(args)...)) {}

  template <std::size_t... Is>
  return_type call_fn_impl_(std::index_sequence<Is...>) {
    // TODO: add std::forward?
    return fn_(std::get<Is>(args_).get()...);
  }

  return_type get() {
    return call_fn_impl_(std::index_sequence_for<Args...>());
  }

private:
  Fn fn_;
  std::tuple<Args...> args_;
};

template <typename Fn, typename... Args>
with_future(Fn fn, Args... args) -> with_future<Fn, Args...>;

} // end futar
