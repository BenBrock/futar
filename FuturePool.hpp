#pragma once

#include <type_traits>
#include <utility>
#include <list>
#include <optional>

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
