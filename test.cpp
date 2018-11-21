#include <iostream>
#include <future.hpp>
#include <future_then.hpp>

int foo(int x) {
  return x;
}

int add(int x, int y) {
  return x + y;
}

int main(int argc, char** argv) {
  auto f = expl::future(foo, 12);

  auto f2 = expl::future_then(add, f, 13);

  auto hey = f.get();

  auto hey2 = f2.get();

  std::cout << hey << std::endl;
  std::cout << hey2 << std::endl;
  return 0;
}
