#pragma once

#include <list>
#include <memory>
#include <limits>
#include <cassert>

#include "../future_chain.hpp"

namespace futar {

template <typename T>
class FuturePool {
public:
  FuturePool() = default;
  FuturePool(size_t capacity) : capacity_(capacity) {}

  T get() {
    if (values_.size() == 0) {
      progress(1);
      assert(values_.size() > 0);
    }
    T rv = values_.front();
    values_.erase(values_.begin());
    return rv;
  }

  std::size_t progress(std::size_t num_to_empty = std::numeric_limits<size_t>::max()) {
    std::size_t num_emptied = 0;
    while (num_emptied < num_to_empty && futures_.size() > 0) {
      for (auto it = futures_.begin(); it != futures_.end(); ) {
        if ((*it)->is_ready()) {
          values_.push_back((*it)->get());
          auto old = it++;
          futures_.erase(old);
          num_emptied++;
          /*
            XXX: To short-circuit or not?
                 Probably better to not; more progress.
          if (num_emptied >= num_to_empty) {
            return num_emptied;
          }
          */
        }
      }
    }
    return num_emptied;
  }

  size_t size() const {
    return values_.size() + futures_.size();
  }

  auto size_() const {
    return std::make_tuple(values_.size(), futures_.size());
  }

  size_t capacity() const {
    return capacity_;
  }

  template <typename U>
  void push_back(U&& future) {
    if (futures_.size() >= capacity()) {
      progress(1);
    }
    future_chain chain(std::move(future));
    futures_.emplace_back(chain.move());
  }

  void drain() {
    while (futures_.size() > 0) {
      progress();
    }
  }

  void get_drain() {
    for (auto it = futures_.begin(); it != futures_.end(); it++) {
      values_.push_back((*it)->get());
    }
    futures_.clear();
  }

private:
  std::list<std::unique_ptr<futar::abstract_future_chain<T>>> futures_;
  std::list<T> values_;
  size_t capacity_ = std::numeric_limits<size_t>::max();
};

} // end futar
