#pragma once

#include <optional>

#include "fn_wrapper.hpp"
#include "future_chain.hpp"

namespace futar {

template <typename Cond, typename Fn, typename... Args>
class while_ {
public:
  using chain_type_t = decltype(future_chain((std::declval<Fn>()(std::declval<Args>()...))));

  while_(Cond&& cond, Fn&& fn, Args&&... args) : cond_(std::move(cond)),
                                                 fn_(std::move(fn)),
                                                 args_(std::move(args)...)
  {
    if (call_fn_impl_(cond_, std::index_sequence_for<Args...>())) {
      future_chain_.emplace(apply_fn_impl_(fn_, std::index_sequence_for<Args...>()));
    } else {
      ready_ = true;
    }
  }

  void progress() {
    if (BCL::rank() == 0) {
      // printf("(%lu) progress... ready? %d\n", BCL::rank(), ready_);
    }
    if (!ready_) {
      if (future_chain_.value().wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        rv_ = future_chain_.value().get();

        if (call_fn_impl_(cond_, std::index_sequence_for<Args...>())) {
          future_chain_.emplace(apply_fn_impl_(fn_, std::index_sequence_for<Args...>()));
        } else {
          ready_ = true;
        }
      } else {
        if (BCL::rank() == 0) {
          // printf("(%lu) not ready yet.\n", BCL::rank()));
        }
      }
    }
  }

  while_(const while_&) = delete;
  while_(while_&&) = default;

  using return_type = std::remove_cv_t<std::remove_reference_t<decltype(get_val_(std::declval<Fn>()(get_val_(std::declval<Args>())...)))>>;

  template <typename Fn_, std::size_t... Is>
  auto apply_fn_impl_(Fn_&& fn, std::index_sequence<Is...>) {
    return future_chain(fn(std::get<Is>(args_)...));
  }

  template <typename Fn_, std::size_t... Is>
  auto call_fn_impl_(Fn_&& fn, std::index_sequence<Is...>) {
    return future_chain(fn(std::get<Is>(args_)...)).get();
  }

  return_type get() {
    while (!ready_) {
      progress();
    }
    return rv_;
  }

  template <class Rep, class Period>
  std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) {
    progress();
    if (ready_) {
      return std::future_status::ready;
    } else {
      return std::future_status::timeout;
    }
  }

private:
  Cond cond_;
  Fn fn_;
  std::tuple<Args...> args_;
  std::optional<chain_type_t> future_chain_;
  return_type rv_;
  bool ready_ = false;
};

template <typename Cond, typename Fn, typename... Args>
while_(Cond&& cond, Fn&& fn, Args&&... args) -> while_<Cond, Fn, Args...>;

} // end futar
