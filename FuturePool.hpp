#pragma once

#include <type_traits>
#include <utility>
#include <list>
#include <optional>
#include <chrono>
#include <cassert>
#include <unistd.h>

namespace expl {

template <
          typename FutureType,
          typename = std::enable_if_t<
                                      !std::is_same<
                                                    void,
                                                    decltype(std::declval<FutureType>().get())
                                                    >::value
                                     >
         >
class FuturePool {
public:
  using future_type = FutureType;
  using return_type = decltype(std::declval<future_type>().get());

  FuturePool() = default;
  FuturePool(const FuturePool&) = delete;
  FuturePool(FuturePool&&) = delete;

  FuturePool(future_type&& future) {
    attach(std::move(future));
  }

  void attach(future_type&& future) {
    futures_.push_back(std::move(future));
  }

  std::list<return_type> finish_all() {
    std::list<return_type> return_values;
    for (auto it = futures_.begin(); it != futures_.end(); it++) {
      return_values.push_back((*it).get());
    }
    futures_.clear();
    return return_values;
  }

  std::optional<return_type> finish_one() {
    if (empty()) {
      return {};
    } else {
      auto it = futures_.begin();
      auto val = (*it).get();
      futures_.erase(it);
      return val;
    }
  }

  std::list<return_type> finish_some() {
    if (empty()) {
      return {};
    }

    bool success = wait_some_();
    assert(success);

    return finish_ready();
  }

  std::list<return_type> finish_ready() {
    std::list<return_type> return_values;
    for (auto it = futures_.begin(); it != futures_.end(); ) {
      if ((*it).wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        auto val = (*it).get();
        return_values.push_back(val);
        auto next_it = it;
        next_it++;
        futures_.erase(it);
        it = next_it;
      } else {
        it++;
      }
    }
    return return_values;
  }

  bool some_ready_() {
    for (auto& future : futures_) {
      if (future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
        return true;
      }
    }
    return false;
  }

  bool wait_some_() {
    if (empty()) {
      return false;
    }

    bool success = false;
    std::size_t sleep_time = 1;
    // max sleep time in between checks 10ms
    std::size_t max_sleep = 10000;

    while (!(success = some_ready_())) {
      usleep(sleep_time);
      sleep_time *= 2;
      sleep_time = std::min(sleep_time, max_sleep);
    }
    return success;
  }

  template <class Rep, class Period>
  std::future_status wait_for(const std::chrono::duration<Rep,Period>& timeout_duration) const {
    auto begin = std::chrono::high_resolution_clock::now();

    auto end = begin;

    bool success = false;
    std::size_t sleep_time = 1;
    // max sleep time in between checks 10ms
    std::size_t max_sleep = 10000;

    while (!(success = some_ready_()) && (end - begin) < timeout_duration) {
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

  std::size_t size() const noexcept {
    return futures_.size();
  }

  bool empty() const noexcept {
    return futures_.empty();
  }

private:
  std::list<future_type> futures_;
};

template <typename FutureType>
FuturePool(FutureType&& future) -> FuturePool<FutureType>;

} // end expl
