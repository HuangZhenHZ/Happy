#pragma once

#include <cmath>

// TODO: Optimize this function
double NormalizeAngle(double x) {
  x = std::fmod(x, 2.0 * M_PI);
  if (x >= M_PI) {
    return x - 2.0 * M_PI;
  } else if (x < -M_PI) {
    return x + 2.0 * M_PI;
  } else {
    return x;
  }
}

template <typename T>
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
  [[nodiscard]] Vec3 Normalized() const {
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


template <typename T>
struct Vec2 {
  static_assert(std::is_floating_point_v<T>);
  T x = 0.0, y = 0.0;
  constexpr Vec2() = default;
  constexpr Vec2(T x, T y) : x(x), y(y) {}
  static Vec2 FromUnit(T heading) {
    return Vec2(std::cos(heading), std::sin(heading));
  }
  T LengthSqr() const {
    return x * x + y * y;
  }
  T Length() const {
    return std::sqrt(x * x + y * y);
  }
  T Angle() const {
    return std::atan2(y, x);
  }
  void Normalize() {
    const T inv = 1.0 / Length();
    x *= inv;
    y *= inv;
  }
  [[nodiscard]] Vec2 Normalized() const {
    const T inv = 1.0 / Length();
    return Vec2(x * inv, y * inv);
  }

  T InnerProd(const Vec2& v) const {
    return x * v.x + y * v.y;
  }
  T CrossProd(const Vec2& v) const {
    return x * v.y - y * v.x;
  }

  Vec2 operator+ (const Vec2& v) const {
    return Vec2(x + v.x, y + v.y);
  }
  Vec2 operator- (const Vec2& v) const {
    return Vec2(x - v.x, y - v.y);
  }
  Vec2 operator* (T t) const {
    return Vec2(x * t, y * t);
  }
  Vec2 operator/ (T t) const {
    return Vec2(x / t, y / t);
  }
  friend Vec2 operator* (T t, const Vec2& v) {
    return Vec2(v.x * t, v.y * t);
  }
  Vec2 operator- () const {
    return Vec2(-x , -y);
  }

  Vec2& operator+= (const Vec2& v) {
    x += v.x;
    y += v.y;
    return *this;
  }
  Vec2& operator-= (const Vec2& v) {
    x -= v.x;
    y -= v.y;
    return *this;
  }
  Vec2& operator*= (T t) {
    x *= t;
    y *= t;
    return *this;
  }
  Vec2& operator/= (T t) {
    x /= t;
    y /= t;
    return *this;
  }

  [[nodiscard]] Vec2 RotatedByUnit(double cos_angle, double sin_angle) const {
    return Vec2(x * cos_angle - y * sin_angle,
                x * sin_angle + y * cos_angle);
  }
  [[nodiscard]] Vec2 Rotated90() const {
    return Vec2(-y, x);
  }
  [[nodiscard]] Vec2 Rotated180() const {
    return Vec2(-x, -y);
  }
  [[nodiscard]] Vec2 Rotated270() const {
    return Vec2(y, -x);
  }

  void RotateByUnit(double cos_angle, double sin_angle) {
    *this = RotatedByUnit(cos_angle, sin_angle);
  }
  void Rotate90() {
    *this = Rotated90();
  }
  void Rotate180() {
    *this = Rotated180();
  }
  void Rotate270() {
    *this = Rotated270();
  }
};

using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;

static_assert(sizeof(Vec2f) == sizeof(float) * 2);
static_assert(sizeof(Vec2d) == sizeof(double) * 2);
