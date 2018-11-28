#include "fn_wrapper.hpp"
#include "pools/VariantPool.hpp"
#include <list>
#include <memory>
#include <iostream>
#include <variant>

template <typename T>
T identity(T value) {
  sleep(1);
  return value;
}

template <typename T>
T add(T a, T b) {
  return a + b;
}

int main(int argc, char** argv) {
  // auto result = futar::apply(add<int>, std::async(identity<int>, 12), std::async(identity<int>, 13));

  // auto result = futar::fn_wrapper(add<int>, std::async(identity<int>, 12), std::async(identity<int>, 13));

  futar::VariantPool<int, float> pool;

  auto result = futar::fn_wrapper([](auto a, auto b) { return a + b; },
                                  std::async(identity<int>, 12), std::async(identity<int>, 13));

  pool.push_back(std::move(result));

  auto fresult = futar::fn_wrapper([](auto a, auto b) { return a + b; },
                                   std::async(identity<float>, 1.23), std::async(identity<float>, 13));

  pool.push_back(std::move(fresult));

  while (pool.size() > 0) {
    auto value = pool.get();

    if (value.index() == 0) {
      std::cout << std::get<0>(value) << std::endl;
    } else if (value.index() == 1) {
      std::cout << std::get<1>(value) << std::endl;
    }
  }

  return 0;
}
