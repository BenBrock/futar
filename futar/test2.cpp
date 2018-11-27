#include "fn_wrapper.hpp"
#include <iostream>

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

  auto result = futar::fn_wrapper([](auto a, auto b) { return a + b; },
                                  std::async(identity<int>, 12), std::async(identity<int>, 13));

  while (true) {
    if (result.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
      std::cout << result.get() << std::endl;
      break;
    } else {
      std::cout << "not ready" << std::endl;
    }
    usleep(100000);
  }
  return 0;
}
