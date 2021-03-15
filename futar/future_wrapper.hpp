#pragma once

#include <future>
#include <futar/future.hpp>
#include <futar/detail/detail.hpp>
#include <utility>

namespace futar {

template <typename T, typename = decay_future_t<T>>
class future_wrapper final : public virtual future<decay_future_t<T>> {
public:
  using return_type = decay_future_t<T>;

  future_wrapper(T&& future) : future_(std::move(future)) {}
  future_wrapper(const future_wrapper&) = delete;
  future_wrapper(future_wrapper&&) = default;
  ~future_wrapper() override = default;

  return_type get() override {
    return std::move(future_.get());
  }

  future_wrapper* move() override {
    return new future_wrapper(std::move(*this));
  }

  template <class Rep, class Period>
  std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) const {
    auto status = future_.wait_for(timeout_duration);
    if (status != std::future_status::timeout) {
      // XXX: We count std::future_status::deferred as ready.
      return std::future_status::ready;
    } else {
      return std::future_status::timeout;
    }
  }

private:
  T future_;
};

} //end futar
