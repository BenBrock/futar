#pragma once

namespace expl {

template <typename T>
class wrapper_future {
public:
  using return_type = T;

  wrapper_future(const T& value) : value_(value) {}
  wrapper_future(T&& value) : value_(std::move(value)) {}
  wrapper_future(const wrapper_future&) = default;
  wrapper_future(wrapper_future&&) = default;

  return_type get() {
    return value_;
  }

  template <class Rep, class Period>
  std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) const {
    return std::future_status::ready;
  }

private:
  T value_;
};

template <typename T>
wrapper_future(const T& value) -> wrapper_future<T>;

} // end expl
