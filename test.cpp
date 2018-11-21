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
  FuturePool<decltype(expl::future(foo, 12))> pool;

  auto f = expl::future(foo, 12);

  pool.attach(expl::future(foo, 12));
  pool.attach(expl::future(foo, 14));
  pool.attach(expl::future(foo, 17));

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
