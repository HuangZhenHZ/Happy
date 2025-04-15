#include "Eigen/Core"
#include "math/vec.h"
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

  std::vector<Vec2d> points;
  for (int i = 0; i < 10; ++i) {
    points.emplace_back(i, i + 1);
  }
  for (const Vec2d& point : points) {
    printf("%lf %lf\n", point.x, point.y);
  }
  putchar('\n');
  Eigen::Map<Eigen::Matrix2Xd> mp(&points[0].x, 2, points.size());
  Eigen::Matrix2d mat;
  mat << 0, 1, 1, 0;
  Eigen::Vector2d vec(2, 4);
  /*
  mp.colwise() -= vec;
  mp.applyOnTheLeft(mat);
  mp.colwise() += vec;
  */
  mp = (mat * (mp.colwise() - vec)).colwise() + vec;
  for (const Vec2d& point : points) {
    printf("%lf %lf\n", point.x, point.y);
  }
  return 0;
}
