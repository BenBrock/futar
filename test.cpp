#include <iostream>
#include <future.hpp>
#include <future_then.hpp>
#include <FuturePool.hpp>

int foo(int x) {
  sleep(x);
  return x;
}

int add(int x, int y) {
  return x + y;
}

int main(int argc, char** argv) {
  auto f = expl::future_then(add, std::async(foo, 12), 13);

  expl::FuturePool pool(std::move(f));

  size_t n_jobs = 10000;

  for (size_t i = 0; i < n_jobs; i++) {
    auto f = expl::future_then(add, std::async(std::launch::async, foo, (int) (lrand48() % 10)), (int) (lrand48() % 10));
    pool.attach(std::move(f));
  }

  std::cout << "Pool is size " << pool.size() << std::endl;

  while (!pool.empty()) {
    auto results = pool.finish_some();
    std::cout << "Got " << results.size() << std::endl;
  }

  std::cout << "Pool is size " << pool.size() << std::endl;

  return 0;
}
