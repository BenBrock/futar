#pragma once

#include <future>
#include <utility>

namespace futar {

template <typename T, typename = decltype(std::declval<T>().get())>
class future_wrapper {
public:
  using return_type = decltype(std::declval<T>().get());

  future_wrapper(T&& future) : future_(std::move(future)) {}
  future_wrapper(const future_wrapper&) = delete;
  future_wrapper(future_wrapper&&) = default;

  return_type get() {
    return std::move(future_.get());
  }

  template <class Rep, class Period>
  std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) const {
    return future_.wait_for(timeout_duration);
  }

private:
  T future_;
};

} //end futar
