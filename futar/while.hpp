#pragma once

#include "fn_wrapper.hpp"

namespace futar {

template <typename Cond, typename Fn, typename... Args>
class while_ {
public:
  while_(Cond&& cond, Fn&& fn, Args&&... args) : cond_(std::move(cond)),
                                                 fn_(std::move(fn)),
                                                 args_(std::move(args)...)
                                               {}

  while_(const while_&) = delete;
  while_(while_&&) = default;

  using return_type = std::remove_cv_t<std::remove_reference_t<decltype(get_val_(std::declval<Fn>()(get_val_(std::declval<Args>())...)))>>;

  template <typename Fn_, std::size_t... Is>
  auto call_fn_impl_(Fn_&& fn, std::index_sequence<Is...>) {
    return get_val_(fn(get_val_(std::get<Is>(args_))...));
  }

  return_type get() {
    while (true) {
      return_type rv = call_fn_impl_(fn_, std::index_sequence_for<Args...>());
      if (!call_fn_impl_(cond_, std::index_sequence_for<Args...>())) {
        return rv;
      } else {
      }
    }
  }

  template <class Rep, class Period>
  std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) const {
    return std::future_status::ready;
  }

private:
  Cond cond_;
  Fn fn_;
  std::tuple<Args...> args_;
};

template <typename Cond, typename Fn, typename... Args>
while_(Cond&& cond, Fn&& fn, Args&&... args) -> while_<Cond, Fn, Args...>;

} // end futar
