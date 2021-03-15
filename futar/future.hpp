#pragma once

#include <utility>
#include <future>
#include <tuple>
#include <chrono>
#include <initializer_list>
#include <futar/detail/detail.hpp>

namespace futar {

template <typename T>
class future {
public:

  virtual T get() = 0;
  virtual future* move() = 0;
  virtual ~future() = default;
};

} // end futar
