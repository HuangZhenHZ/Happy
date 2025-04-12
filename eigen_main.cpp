#include "Eigen/Core"
// #include "fmt/core.h"
#include <cstdio>

int main() {
  Eigen::Array<double, 10, 1> a, b;
  for (int i = 0; i < 10; ++i) {
    a(i) = i;
    b(i) = i * i;
  }
  auto c = a + b;
  for (int i = 0; i < 10; ++i) {
    // fmt::print("{}\n", c(i));
    printf("%lf\n", c(i));
  }
  return 0;
}
