#pragma once

#include "vec.h"

template <typename FloatType>
class Segment2 {
  using VecType = Vec2<FloatType>;
  VecType start_;
  VecType end_;
public:
  Segment2() = default;
  Segment2(const VecType& start, const VecType& end) : start_(start), end_(end) {}

  const VecType& start() const { return start_; }
  const VecType& end() const { return end_; }
  VecType midpoint() const { return 0.5 * (start_ + end_); }
  FloatType min_x() const { return std::min(start_.x(), end_.x()); }
  FloatType max_x() const { return std::max(start_.x(), end_.x()); }
  FloatType min_y() const { return std::min(start_.y(), end_.y()); }
  FloatType max_y() const { return std::max(start_.y(), end_.y()); }
  FloatType LengthSqr() const {
    return ::LengthSqr(end_ - start_);
  }
  FloatType Length() const {
    return ::Length(end_ - start_);
  }
  FloatType IsZeroLength() const {
    return ::LengthSqr(end_ - start_) < 1e-18;
  }
  FloatType DistanceToPoint(const VecType& point) const {
    if (IsZeroLength()) {
      return ::Length(midpoint() - point);
    }
    if (InnerProd(end_ - start_, point - start_) < 0) {
      return ::Length(point - start_);
    }
    if (InnerProd(start_ - end_, point - end_) < 0) {
      return ::Length(point - end_);
    }
    return std::abs(CrossProd(end_ - start_, point - start_)) / Length();
  }

  bool HasIntersectWithSegment(const Segment2& other_segment) const {
    if (min_x() > other_segment.max_x() || max_x() < other_segment.min_x() ||
        min_y() > other_segment.max_y() || max_y() < other_segment.min_y()) {
      return false;
    }
    FloatType c1 = CrossProd(end_ - start_, other_segment.start() - start_);
    FloatType c2 = CrossProd(end_ - start_, other_segment.end() - start_);
    if (c1 * c2 > 0) {
      return false;
    }
    FloatType c3 = CrossProd(other_segment.end() - other_segment.start(), start_ - other_segment.start());
    FloatType c4 = CrossProd(other_segment.end() - other_segment.start(), end_ - other_segment.start());
    if (c3 * c4 > 0) {
      return false;
    }
    return true;
  }
};

using Segment2d = Segment2<double>;
