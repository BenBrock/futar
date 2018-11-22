#include <iostream>
#include <future.hpp>
#include <future_then.hpp>
#include <FuturePool.hpp>

#include "with_future.hpp"
#include "wrapper_future.hpp"

int foo(int x) {
  return x;
}

int add(int x, int y) {
  return x + y;
}

int main(int argc, char** argv) {

  auto f = futar::with_future(add, futar::future(foo, 12), futar::future(foo, 13));
  auto f2 = futar::with_future(add, f, futar::wrapper_future(13));

  auto val = f2.get();
  std::cout << val << std::endl;

  /*
  auto f = futar::future_then(add, std::async(foo, 12), 13);

  futar::FuturePool pool(std::move(f));

  size_t n_jobs = 1000;

  for (size_t i = 0; i < n_jobs; i++) {
    auto f = futar::future_then(add, std::async(foo, (int) (lrand48() % 10)), (int) (lrand48() % 10));
    pool.attach(std::move(f));
  }

  std::cout << "Pool is size " << pool.size() << std::endl;

  while (!pool.empty()) {
    auto results = pool.finish_some();
    std::cout << "Got " << results.size() << std::endl;
  }

  std::cout << "Pool is size " << pool.size() << std::endl;
  */

  return 0;
}
