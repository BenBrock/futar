#include <iostream>
#include <future.hpp>
#include <future_then.hpp>
#include <FuturePool.hpp>

int foo(int x) {
  return x;
}

int add(int x, int y) {
  return x + y;
}

int main(int argc, char** argv) {
  auto f = std::async(foo, 12);

  auto f2 = expl::future_then(add, std::move(f), 13);

  expl::FuturePool pool(std::move(f2));

  std::cout << "Pool is size " << pool.size() << std::endl;
  auto result = pool.finish_one();
  std::cout << "Got " << result.value() << std::endl;
  std::cout << "Pool is size " << pool.size() << std::endl;

  auto results = pool.finish_all();

  for (const auto& result : results) {
    std::cout << "Got " << result << std::endl;
  }

  std::cout << "Pool is size " << pool.size() << std::endl;

  return 0;
}
