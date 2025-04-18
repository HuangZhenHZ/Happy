#pragma once

#include "segment2d.h"

#include <array>

template <typename FloatType>
class Box2 {
  using VecType = Vec2<FloatType>;
  using SegmentType = Segment2<FloatType>;
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
    const FloatType abs_cos_heading = std::abs(heading_.x());
    const FloatType abs_sin_heading = std::abs(heading_.y());
    const FloatType dx = abs_cos_heading * half_length_ + abs_sin_heading * half_width_;
    const FloatType dy = abs_cos_heading * half_width_ + abs_sin_heading * half_length_;
    min_x_ = center_.x() - dx;
    max_x_ = center_.x() + dx;
    min_y_ = center_.y() - dy;
    max_y_ = center_.y() + dy;
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

  FloatType area() const { return length_ * width_; }

  std::array<VecType, 4> GetCornersArray() const {
    VecType half_length_vec = half_length_ * heading_;
    VecType half_width_vec = half_width_ * Rotated90(heading_);
    return {
      center_ + half_length_vec + half_width_vec,
      center_ - half_length_vec + half_width_vec,
      center_ - half_length_vec - half_width_vec,
      center_ + half_length_vec - half_width_vec,
    };
  }

  bool IsPointInOrOnBoundary(const VecType& point) const {
    const FloatType abs_inner = std::abs(InnerProd(heading_, point - center_));
    const FloatType abs_cross = std::abs(CrossProd(heading_, point - center_));
    return abs_inner <= half_length_ + 1e-9 && abs_cross <= half_width_ + 1e-9;
  }

  FloatType DistanceToPoint(const VecType& point) const {
    const FloatType abs_inner = std::abs(InnerProd(heading_, point - center_));
    const FloatType abs_cross = std::abs(CrossProd(heading_, point - center_));
    return ApproxHypot(std::max(abs_inner - half_length_, 0.0),
                       std::max(abs_cross - half_width_, 0.0));
  }

  FloatType DistanceSqrToPoint(const VecType& point) const {
    const FloatType abs_inner = std::abs(InnerProd(heading_, point - center_));
    const FloatType abs_cross = std::abs(CrossProd(heading_, point - center_));
    FloatType x = std::max(abs_inner - half_length_, 0.0);
    FloatType y = std::max(abs_cross - half_width_, 0.0);
    return x * x + y * y;
  }

  bool HasOverlapWithSegment(const SegmentType& segment) const {
    VecType segment_midpoint = segment.midpoint();
    VecType center_to_midpoint = segment_midpoint - center_;
    FloatType midpoint_inner_prod = InnerProd(heading_, center_to_midpoint);
    FloatType midpoint_cross_prod = CrossProd(heading_, center_to_midpoint);

    VecType segment_vec = segment.end() - segment.start();
    FloatType segment_vec_inner_prod = InnerProd(heading_, segment_vec);
    FloatType segment_vec_cross_prod = CrossProd(heading_, segment_vec);

    return std::abs(midpoint_inner_prod) * 2 <= std::abs(segment_vec_inner_prod) + length_ + 1e-9 &&
           std::abs(midpoint_cross_prod) * 2 <= std::abs(segment_vec_cross_prod) + width_ + 1e-9 &&
        std::abs(segment_vec_inner_prod * midpoint_cross_prod -
                 segment_vec_cross_prod * midpoint_inner_prod) * 2 <=
            std::abs(segment_vec_inner_prod) * width_ + std::abs(segment_vec_cross_prod) * length_ + 1e-9;
  }

  FloatType DistanceSqrToSegment(const SegmentType& segment) const {
    VecType segment_midpoint = segment.midpoint();
    VecType center_to_midpoint = segment_midpoint - center_;
    FloatType midpoint_inner_prod = InnerProd(heading_, center_to_midpoint);
    FloatType midpoint_cross_prod = CrossProd(heading_, center_to_midpoint);

    VecType segment_vec = segment.end() - segment.start();
    FloatType segment_vec_inner_prod = InnerProd(heading_, segment_vec);
    FloatType segment_vec_cross_prod = CrossProd(heading_, segment_vec);

    if (std::abs(midpoint_inner_prod) * 2 <= std::abs(segment_vec_inner_prod) + length_ &&
        std::abs(midpoint_cross_prod) * 2 <= std::abs(segment_vec_cross_prod) + width_ &&
        std::abs(segment_vec_inner_prod * midpoint_cross_prod -
                 segment_vec_cross_prod * midpoint_inner_prod) * 2 <=
            std::abs(segment_vec_inner_prod) * width_ + std::abs(segment_vec_cross_prod) * length_) {
      return 0.0;
    }

    if (segment.IsZeroLength()) {
      return DistanceSqrToPoint(segment_midpoint);
    }

    VecType segment_start_to_box_center = center_ - segment.start();
    FloatType center_inner_prod = InnerProd(segment_vec, segment_start_to_box_center);
    FloatType center_cross_prod = CrossProd(segment_vec, segment_start_to_box_center);
    FloatType length_inner_prod = segment_vec_inner_prod * half_length_;
    FloatType length_cross_prod = -segment_vec_cross_prod * half_length_;
    FloatType width_inner_prod = segment_vec_cross_prod * half_width_;
    FloatType width_cross_prod = segment_vec_inner_prod * half_width_;
    FloatType segment_length_sqr = InnerProd(segment_vec, segment_vec);

    FloatType min_abs_cross_prod = std::numeric_limits<FloatType>::infinity();

    FloatType corner_1_inner_prod = center_inner_prod + length_inner_prod + width_inner_prod;
    if (corner_1_inner_prod > 0.0 && corner_1_inner_prod < segment_length_sqr) {
      min_abs_cross_prod = std::min(min_abs_cross_prod,
          std::abs(center_cross_prod + length_cross_prod + width_cross_prod));
    }

    FloatType corner_2_inner_prod = center_inner_prod + length_inner_prod - width_inner_prod;
    if (corner_2_inner_prod > 0.0 && corner_2_inner_prod < segment_length_sqr) {
      min_abs_cross_prod = std::min(min_abs_cross_prod,
          std::abs(center_cross_prod + length_cross_prod - width_cross_prod));
    }

    FloatType corner_3_inner_prod = center_inner_prod - length_inner_prod + width_inner_prod;
    if (corner_3_inner_prod > 0.0 && corner_3_inner_prod < segment_length_sqr) {
      min_abs_cross_prod = std::min(min_abs_cross_prod,
          std::abs(center_cross_prod - length_cross_prod + width_cross_prod));
    }

    FloatType corner_4_inner_prod = center_inner_prod - length_inner_prod - width_inner_prod;
    if (corner_4_inner_prod > 0.0 && corner_4_inner_prod < segment_length_sqr) {
      min_abs_cross_prod = std::min(min_abs_cross_prod,
          std::abs(center_cross_prod - length_cross_prod - width_cross_prod));
    }

    FloatType min_distance_sqr = min_abs_cross_prod * min_abs_cross_prod / segment_length_sqr;
    min_distance_sqr = std::min(min_distance_sqr, DistanceSqrToPoint(segment.start()));
    min_distance_sqr = std::min(min_distance_sqr, DistanceSqrToPoint(segment.end()));
    return min_distance_sqr;
  }

  FloatType DistanceToSegment(const SegmentType& segment) const {
    return std::sqrt(DistanceSqrToSegment(segment));
  }

  bool HasOverlapWithBox(const Box2& box) const {
    const VecType center_diff = box.center() - center_;
    const FloatType abs_inner_prod = std::abs(InnerProd(heading_, box.heading()));
    const FloatType abs_cross_prod = std::abs(CrossProd(heading_, box.heading()));
    return std::abs(InnerProd(heading_, center_diff)) <=
            half_length_ + abs_inner_prod * box.half_length() +
                           abs_cross_prod * box.half_width() + 1e-9 &&
        std::abs(CrossProd(heading_, center_diff)) <=
            half_width_ + abs_inner_prod * box.half_width() +
                          abs_cross_prod * box.half_length() + 1e-9 &&
        std::abs(InnerProd(box.heading(), center_diff)) <=
            box.half_length() + abs_inner_prod * half_length_ +
                                abs_cross_prod * half_width_ + 1e-9 &&
        std::abs(CrossProd(box.heading(), center_diff)) <=
            box.half_width() + abs_inner_prod * half_width_ +
                               abs_cross_prod * half_length_ + 1e-9;
  }
};

using Box2d = Box2<double>;
