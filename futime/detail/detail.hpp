#pragma once

#include <utility>

namespace expl {

template <typename T>
struct decay_future {
  using type = decltype(std::declval<T>().get());
};

} // end expl
