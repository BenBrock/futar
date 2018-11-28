#pragma once

#include <memory>
#include <variant>
#include <list>

namespace futar {

template <typename... Args>
struct VariantPool {
  using future_variant_t = std::variant<std::unique_ptr<futar::future<Args>>...>;

  template <std::size_t I = 0>
  void unpack_future_variant_impl_(std::variant<Args...>& rv, future_variant_t& fv) {
    static_assert(I < sizeof...(Args));
    if (fv.index() == I) {
      rv = std::get<I>(fv)->get();
      return;
    }

    if constexpr(I+1 < sizeof...(Args)) {
      unpack_future_variant_impl_<I+1>(rv, fv);
    }
  }

  auto unpack_future_variant(future_variant_t& fv) {
    std::variant<Args...> rv;
    unpack_future_variant_impl_<0>(rv, fv);
    return rv;
  }

  std::variant<Args...> get() {
    auto rv = unpack_future_variant(futures_.front());
    futures_.erase(futures_.begin());
    return rv;
  }

  size_t size() const {
    return futures_.size();
  }

  template <typename T>
  void push_back(futar::future<T>&& future) {
    futures_.emplace_back(std::unique_ptr<futar::future<T>>(future.move()));
  }

  // Could be a future<T> for any T in Args
  std::list<future_variant_t> futures_;
};

} // end futar
