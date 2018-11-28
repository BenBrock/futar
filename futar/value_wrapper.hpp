#pragma once

#include <future>
#include "future.hpp"

namespace futar {

// Wrap a value in a future type.
template <typename T>
class value_wrapper final : public virtual future<T> {
public:
  using return_type = T;

  value_wrapper() = delete;
  value_wrapper(const T& value) : value_(value) {}
  value_wrapper(T&& value) : value_(std::move(value)) {}
  value_wrapper(const value_wrapper&) = default;
  value_wrapper(value_wrapper&&) = default;
  ~value_wrapper() override = default;

  return_type get() override {
    return std::move(value_);
  }

  value_wrapper* move() override {
    return new value_wrapper(std::move(*this));
  }

  template <class Rep, class Period>
  std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) const {
    return std::future_status::ready;
  }

private:
  T value_;
};

template <typename T>
value_wrapper(const T& value) -> value_wrapper<T>;

template <typename T>
value_wrapper(T&& value) -> value_wrapper<T>;

} // end futar
