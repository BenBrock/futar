#include "fn_wrapper.hpp"
#include "future_chain.hpp"
#include <list>
#include <memory>
#include <iostream>
#include <variant>
#include <pools/ChainPool.hpp>
#include "while.hpp"

template <typename T>
T identity(T value) {
  sleep(lrand48() % 5);
  return value;
}

template <typename T>
T add(T a, T b) {
  return a + b;
}

int main(int argc, char** argv) {

  int val = 0;
  futar::while_([](int&& val) { return val < 10; }, [](int&& val) { val++; }, std::move(val));
  std::cout << "val is " << val << std::endl;
  /*
  futar::FuturePool<int> pool(10);

  size_t num_to_push = 100;

  for (size_t i = 0; i < num_to_push; i++) {
    auto result = futar::call([](auto a, auto b) { return std::async(add<int>, a, b); },
                                 std::async(identity<int>, 12), std::async(identity<int>, 13));
    pool.push_back(std::move(result));
  }

  pool.drain();

  while (pool.size() > 0) {
    std::cout << pool.get() << std::endl;
  }
  */

  return 0;
}
