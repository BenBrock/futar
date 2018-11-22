#include <iostream>
#include <future.hpp>
#include <future_then.hpp>
#include <FuturePool.hpp>

#include "with_future.hpp"
#include "wrapper_future.hpp"

int foo(int x) {
  sleep(x);
  return x;
}

int add(int x, int y) {
  return x + y;
}

int main(int argc, char** argv) {
  auto f = futar::with_future(add, std::move(std::async(foo, lrand48() % 10)), std::move(std::async(foo, lrand48() % 10)));
  auto f2 = futar::with_future(add, std::move(f), futar::wrapper_future(13));

  futar::FuturePool pool(std::move(f2));

  size_t n_jobs = 1000;

  for (size_t i = 0; i < n_jobs; i++) {
    auto f = futar::with_future(add, std::move(std::async(foo, lrand48() % 10)), std::move(std::async(foo, lrand48() % 10)));
    auto f2 = futar::with_future(add, std::move(f), futar::wrapper_future(13));
    pool.attach(std::move(f2));
  }

  std::cout << "Pool is size " << pool.size() << std::endl;

  while (!pool.empty()) {
    auto results = pool.finish_some();
    std::cout << "Got " << results.size() << std::endl;
  }

  std::cout << "Pool is size " << pool.size() << std::endl;

  return 0;
}
