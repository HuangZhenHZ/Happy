#pragma once

#include <cmath>
#include <cassert>

template <class T>
struct Vec3 {
  static_assert(std::is_floating_point_v<T>);
  T x = 0.0, y = 0.0, z = 0.0;
  constexpr Vec3() = default;
  constexpr Vec3(T x, T y, T z) : x(x), y(y), z(z) {}
  T LengthSqr() const {
    return x * x + y * y + z * z;
  }
  T Length() const {
    return std::sqrt(x * x + y * y + z * z);
  }
  void Normalize() {
    const T inv = 1.0 / Length();
    assert(std::isfinite(inv));
    x *= inv;
    y *= inv;
    z *= inv;
  }
  Vec3 Normalized() const {
    const T inv = 1.0 / Length();
    assert(std::isfinite(inv));
    return Vec3(x * inv, y * inv, z * inv);
  }
};

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
