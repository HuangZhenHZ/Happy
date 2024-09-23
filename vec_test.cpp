#include "vec.h"

#include <cassert>
#include <cstdio>
#include <iostream>

template <typename OP, typename T1, typename T2>
bool ExpectOP(T1 x, T2 y, const char* file, int line, const char* xname, const char* yname) {
  if (!OP{}(x, y)) {
    printf("%s:%d Failed\n", file, line);
    std::cout << xname << " = " << x << "\n" << yname << " = " << y << std::endl;
    return false;
  }
  return true;
}

#define EXPECT_EQ(x, y) ExpectOP<std::equal_to<>>(x, y, __FILE__, __LINE__, #x, #y)
#define EXPECT_NE(x, y) ExpectOP<std::not_equal_to<>>(x, y, __FILE__, __LINE__, #x, #y)
#define EXPECT_LT(x, y) ExpectOP<std::less<>>(x, y, __FILE__, __LINE__, #x, #y)
#define EXPECT_LE(x, y) ExpectOP<std::less_equal<>>(x, y, __FILE__, __LINE__, #x, #y)
#define EXPECT_GT(x, y) ExpectOP<std::greater<>>(x, y, __FILE__, __LINE__, #x, #y)
#define EXPECT_GE(x, y) ExpectOP<std::greater_equal<>>(x, y, __FILE__, __LINE__, #x, #y)

int main() {
  EXPECT_EQ(std::string("aaa") + "bbb", "ccc");

  Vec3f a(1, 2, 3);

  Vec3f b = a.Normalized();
  printf("%f %f %f\n", b.x, b.y, b.z);
  assert(std::abs(b.Length() - 1) < 0.01);

  assert(a.x == 1);
  EXPECT_GT(a.x, 3.0);

  a.Normalize();
  printf("%f %f %f\n", a.x, a.y, a.z);
  assert(std::abs(a.Length() - 1) < 0.01);

  puts("OK");
  return 0;
}
