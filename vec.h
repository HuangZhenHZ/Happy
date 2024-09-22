#pragma once

template <class FloatType>
struct Vec3 {
  FloatType x = 0.0, y = 0.0, z = 0.0;
};

using Vec3f = Vec3<float>;
using Vec3d = Vec3<double>;
