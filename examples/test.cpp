#include <iostream>
#include <futar/futar.hpp>

int foo(int x) {
  sleep(x);
  return x;
}

int add(int x, int y) {
  return x + y;
}

int main(int argc, char** argv) {
  auto f = futar::with_future(add, std::move(std::async(foo, lrand48() % 10)), std::move(std::async(foo, lrand48() % 10)));
  auto f2 = futar::with_future(add, std::move(f), futar::value_wrapper(13));

  // At most 10 outstanding operations at a time.
  size_t capacity = 10;

  futar::FuturePool<int> pool(capacity);
  pool.push_back(std::move(f2));

  size_t n_jobs = 100;

  for (size_t i = 0; i < n_jobs; i++) {
    auto f = futar::with_future(add, std::move(std::async(foo, lrand48() % 10)), std::move(std::async(foo, lrand48() % 10)));
    auto f2 = futar::with_future(add, std::move(f), futar::value_wrapper(13));
    pool.push_back(std::move(f2));

    while (pool.ready_size() > 0) {
      std::cout << pool.get() << std::endl;
    }
  }

  pool.drain();

  return 0;
}
