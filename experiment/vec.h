#pragma once

#include "Eigen/Core"

#include <cmath>

// TODO: Optimize this function
inline double NormalizeAngle(double x) {
  x = std::fmod(x, 2.0 * M_PI);
  if (x >= M_PI) {
    return x - 2.0 * M_PI;
  } else if (x < -M_PI) {
    return x + 2.0 * M_PI;
  } else {
    return x;
  }
}

template <typename FloatType>
FloatType ApproxHypot(FloatType x, FloatType y) {
  static_assert(std::is_floating_point_v<FloatType>);
  return std::sqrt(x * x + y * y);
}

template <typename Scalar>
using Vec2 = Eigen::Vector2<Scalar>;

template <typename Scalar>
Vec2<Scalar> FromUnit(Scalar angle) {
  return {std::cos(angle), std::sin(angle)};
}

template <typename T> auto Length(const T &v) { return v.norm(); }
template <typename T> auto LengthSqr(const T &v) { return v.squaredNorm(); }
template <typename T> auto Angle(const T &v) { return std::atan2(v.y(), v.x()); }

template <typename T1, typename T2>
auto InnerProd(const T1 &a, const T2 &b) {
  return a.dot(b);
}

template <typename T1, typename T2>
auto CrossProd(const T1 &a, const T2 &b) {
  return a.x() * b.y() - a.y() * b.x();
}

template <typename T>
T RotatedByUnit(const T &v,
                typename T::Scalar cos_angle,
                typename T::Scalar sin_angle) {
  return {v.x() * cos_angle - v.y() * sin_angle,
          v.x() * sin_angle + v.y() * cos_angle};
}

template <typename T>
void RotateByUnit(T *v,
                  typename T::Scalar cos_angle,
                  typename T::Scalar sin_angle) {
  *v = {v->x() * cos_angle - v->y() * sin_angle,
        v->x() * sin_angle + v->y() * cos_angle};
}

template <typename T> T Rotated90(const T &v) { return {-v.y(), v.x()}; }
template <typename T> T Rotated180(const T &v) { return {-v.x(), -v.y()}; }
template <typename T> T Rotated270(const T &v) { return {v.y(), -v.x()}; }

template <typename T> void Rotate90(T *v) { *v = {-v->y(), v->x()}; }
template <typename T> void Rotate180(T *v) { *v = {-v->x(), -v->y()}; }
template <typename T> void Rotate270(T *v) { *v = {v->y(), -v->x()}; }

using Vec2f = Vec2<float>;
using Vec2d = Vec2<double>;

static_assert(sizeof(Vec2f) == sizeof(float) * 2);
static_assert(sizeof(Vec2d) == sizeof(double) * 2);
