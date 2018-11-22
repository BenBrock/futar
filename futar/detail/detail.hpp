#pragma once

#include <utility>

namespace futar {

template <typename T>
struct decay_future {
  using type = decltype(std::declval<T>().get());
};

} // end futar
