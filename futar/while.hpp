#pragma once

#include "fn_wrapper.hpp"

namespace futar {

template <typename Cond, typename Fn, typename... Args>
class while_ {
public:
  while_(Cond&& cond, Fn&& fn, Args&&... args) : cond_(std::move(cond)),
                                                 fn_(std::move(fn)),
                                                 args_(std::make_tuple(std::move(args)...))
                                               {}

  template <typename Fn_, std::size_t... Is>
  auto call_fn_impl_(Fn_&& fn, std::index_sequence<Is...>) {
    return fn(std::get<Is>(args_).get()...);
  }

  void get() {
    while (call_fn_impl_(cond_, std::index_sequence_for<Args...>())) {
      call_fn_impl_(fn_, std::index_sequence_for<Args...>());
    }
  }
private:
  Cond cond_;
  Fn fn_;
  std::tuple<decltype(wrap_future(std::declval<Args>()))...> args_;
};

} // end futar
