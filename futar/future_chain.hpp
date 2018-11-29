#pragma once

#include <chrono>
#include "detail/detail.hpp"

namespace futar {

template <typename T>
struct abstract_future_chain {
  virtual T get() = 0;
  virtual bool is_ready() = 0;
  virtual abstract_future_chain* move() = 0;
  virtual ~abstract_future_chain() = default;
};

template <typename T>
class future_chain final : public virtual abstract_future_chain<std::remove_cv_t<std::remove_reference_t<decay_chain_t<T>>>> {
public:
  using return_type = decay_chain_t<T>;

  future_chain(T&& future) : future_(std::move(future)) {}

  future_chain(const future_chain&) = delete;
  future_chain(future_chain&&) = default;

  // XXX: my convention for visitor functions is
  //      "return true if you are finished visiting"

  return_type get() override {
    auto get_impl_ = [&](auto&& future) -> bool {
      if constexpr(is_future<decltype(future)>::value) {
        future_ = future.get();
        return false;
      } else {
        return true;
      }
    };
    while (!std::visit(get_impl_, future_)) {}
    return std::get<return_type>(future_);
  }

  void evaluate_ready() {
    auto eval_impl_ = [&](auto&& future) -> bool {
      if constexpr(is_future<decltype(future)>::value) {
        if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
          future_ = future.get();
          return false;
        } else {
          return true;
        }
      }
      return true;
    };
    while (!std::visit(eval_impl_, future_)) {}
  }

  bool is_ready_() const {
    return std::visit([](auto&& future) -> bool {
      if constexpr(is_future<decltype(future)>::value) {
        return false;
      } else {
        return true;
      }
    }, future_);
  }

  bool is_ready() {
    evaluate_ready();
    return is_ready_();
  }

  template <class Rep, class Period>
  std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) {
    auto begin = std::chrono::high_resolution_clock::now();
    auto end = begin;

    bool success = false;
    std::size_t sleep_time = 1;
    // OPTIMIZE: max sleep time in between checks 10ms
    std::size_t max_sleep = 10000;

    while (!(success = is_ready()) && (end - begin) < timeout_duration) {
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

  future_chain* move() override {
    return new future_chain(std::move(*this));
  }

private:
  chain_variant_t<T> future_;
};

template <typename T>
future_chain(T&& future) -> future_chain<T>;

} // end futar
