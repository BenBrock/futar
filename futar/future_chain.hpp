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
  using return_type = std::remove_cv_t<std::remove_reference_t<decay_chain_t<T>>>;

  static constexpr size_t variant_size = std::variant_size<chain_variant_t<T>>::value;

  future_chain(T&& future) : future_(std::move(future)) {}

  future_chain(const future_chain&) = delete;
  future_chain(future_chain&&) = default;

  // XXX: my convention for visitor functions is
  //      "return true if you are finished visiting"

  // XXX: For some reason std::visit() doesn't work
  // properly in all cases here.
  template <std::size_t I = 0>
  bool get_impl_() {
    using current_type = decltype(std::get<I>(future_));
    if constexpr(futar::is_future<current_type>::value) {
      if (future_.index() == I) {
        future_ = std::get<I>(future_).get();
        return false;
      }
    } else {
      if (future_.index() == I) {
        return true;
      }
    }

    if constexpr(I+1 < variant_size) {
      return get_impl_<I+1>();
    }
    std::cout << "THIS SHOULD NOT HAPPEN" << std::endl;
    return true;
  }

  template <std::size_t I = 0>
  bool eval_impl_() {
    using current_type = decltype(std::get<I>(future_));
    if constexpr(futar::is_future<current_type>::value) {
      if (future_.index() == I) {
        if (std::get<I>(future_).wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
          future_ = std::get<I>(future_).get();
          return false;
        } else {
          return true;
        }
      }
    } else {
      if (future_.index() == I) {
        return true;
      }
    }

    if constexpr(I+1 < variant_size) {
      return eval_impl_<I+1>();
    }

    std::cout << "THIS SHOULD NOT HAPPEN" << std::endl;
    return true;
  }

  template <std::size_t I = 0>
  bool is_ready_impl_() {
    using current_type = decltype(std::get<I>(future_));
    if constexpr(futar::is_future<current_type>::value) {
      if (future_.index() == I) {
        return false;
      }
    } else {
      if (future_.index() == I) {
        return true;
      }
    }

    if constexpr(I+1 < variant_size) {
      return is_ready_impl_<I+1>();
    }
    std::cout << "THIS SHOULD NOT HAPPEN" << std::endl;
    return true;
  }

  return_type get() override {
    while (!get_impl_()) {}
    return std::get<return_type>(future_);
  }

  void evaluate_ready() {
    while (!eval_impl_()) {}
  }

  bool is_ready_() {
    return is_ready_impl_();
  }

  bool is_ready() override {
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
