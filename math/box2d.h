#pragma once

#include "vec.h"

#include <array>

template <typename FloatType>
class Box2 {
  using VecType = Vec2<FloatType>;
  VecType center_;
  VecType heading_;
  FloatType length_;
  FloatType width_;

  FloatType half_length_;
  FloatType half_width_;
  FloatType min_x_;
  FloatType max_x_;
  FloatType min_y_;
  FloatType max_y_;

  void Init() {
    half_length_ = length_ * 0.5;
    half_width_ = width_ * 0.5;
    const FloatType abs_cos_heading = std::abs(heading_.x);
    const FloatType abs_sin_heading = std::abs(heading_.y);
    const FloatType dx = abs_cos_heading * half_length_ + abs_sin_heading * half_width_;
    const FloatType dy = abs_cos_heading * half_width_ + abs_sin_heading * half_length_;
    min_x_ = center_.x - dx;
    max_x_ = center_.x + dx;
    min_y_ = center_.y - dy;
    max_y_ = center_.y + dy;
  }

public:
  Box2(const VecType& center, const VecType& heading, FloatType length, FloatType width)
      : center_(center), heading_(heading), length_(length), width_(width) {
    Init();
  }

  VecType center() const { return center_; }
  VecType heading() const { return heading_; }
  FloatType length() const { return length_; }
  FloatType width() const { return width_; }

  FloatType half_length() const { return half_length_; }
  FloatType half_width() const { return half_width_; }
  FloatType min_x() const { return min_x_; }
  FloatType max_x() const { return max_x_; }
  FloatType min_y() const { return min_y_; }
  FloatType max_y() const { return max_y_; }

  std::array<VecType, 4> GetCornersArray() const {
    VecType half_length_vec = half_length_ * heading_;
    VecType half_width_vec = half_width_ * heading_.Rotated90();
    return {
      center_ + half_length_vec + half_width_vec,
      center_ - half_length_vec + half_width_vec,
      center_ - half_length_vec - half_width_vec,
      center_ + half_length_vec - half_width_vec,
    };
  }
};

using Box2d = Box2<double>;
