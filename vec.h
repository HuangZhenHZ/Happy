#pragma once

#include <cmath>

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
    x *= inv;
    y *= inv;
    z *= inv;
  }
  Vec3 Normalized() const {
    const T inv = 1.0 / Length();
    return Vec3(x * inv, y * inv, z * inv);
  }
  T InnerProd(const Vec3& v) {
    return x * v.x + y * v.y + z * v.z;
  }
  Vec3 CrossProd(const Vec3& v) {
    return Vec3(y * v.z - z * v.y,
                z * v.x - x * v.z,
                x * v.y - y * v.x);
  }

  Vec3 operator+ (const Vec3& v) const {
    return Vec3(x + v.x, y + v.y, z + v.z);
  }
  Vec3 operator- (const Vec3& v) const {
    return Vec3(x - v.x, y - v.y, z - v.z);
  }
  Vec3 operator* (T t) const {
    return Vec3(x * t, y * t, z * t);
  }
  Vec3 operator/ (T t) const {
    return Vec3(x / t, y / t, z / t);
  }
  friend Vec3 operator* (T t, const Vec3& v) {
    return Vec3(v.x * t, v.y * t, v.z * t);
  }
  Vec3 operator- () const {
    return Vec3(-x , -y, -z);
  }

  Vec3& operator+= (const Vec3& v) {
    x += v.x;
    y += v.y;
    z += v.z;
    return *this;
  }
  Vec3& operator-= (const Vec3& v) {
    x -= v.x;
    y -= v.y;
    z -= v.z;
    return *this;
  }
  Vec3& operator*= (T t) {
    x *= t;
    y *= t;
    z *= t;
    return *this;
  }
  Vec3& operator/= (T t) {
    x /= t;
    y /= t;
    z /= t;
    return *this;
  }
};

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;

static_assert(sizeof(Vec3f) == sizeof(float) * 3);
static_assert(sizeof(Vec3d) == sizeof(double) * 3);
