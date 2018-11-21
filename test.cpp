#include <iostream>
#include <future.hpp>

int foo(int x) {
  return x;
}

int main(int argc, char** argv) {
  auto f = expl::future(foo, std::make_tuple(12));

  auto hey = f.get();

  std::cout << hey << std::endl;
  return 0;
}
