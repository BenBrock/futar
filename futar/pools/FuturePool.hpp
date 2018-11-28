#pragma once

#include <list>
#include <memory>
#include <limits>

namespace futar {

template <typename T>
class FuturePool {
public:
  FuturePool() = default;
  FuturePool(size_t capacity) : capacity_(capacity) {}

  T get() {
    if (values_.size() > 0) {
      T rv = values_.front();
      values_.erase(values_.begin());
      return rv;
    } else {
      return get_raw_();
    }
  }

  T get_raw_() {
    T rv = futures_.front()->get();
    futures_.erase(futures_.begin());
    return rv;
  }

  size_t size() const {
    return values_.size() + futures_.size();
  }

  size_t capacity() const {
    return capacity_;
  }

  void push_back(futar::future<T>&& future) {
    if (futures_.size() >= capacity()) {
      values_.emplace_back(std::move(get_raw_()));
    }
    futures_.emplace_back(future.move());
  }

  void drain() {
    while (size() > 0) {
      get();
    }
  }

private:
  std::list<std::unique_ptr<futar::future<T>>> futures_;
  std::list<T> values_;
  size_t capacity_ = std::numeric_limits<size_t>::max();
};

} // end futar
