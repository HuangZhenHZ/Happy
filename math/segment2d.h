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
  FloatType min_x() const { return std::min(start_.x, end_.x); }
  FloatType max_x() const { return std::max(start_.x, end_.x); }
  FloatType min_y() const { return std::min(start_.y, end_.y); }
  FloatType max_y() const { return std::max(start_.y, end_.y); }
  FloatType LengthSqr() const {
    return (end_ - start_).LengthSqr();
  }
  FloatType Length() const {
    return (end_ - start_).Length();
  }
  FloatType IsZeroLength() const {
    return (end_ - start_).LengthSqr() < 1e-18;
  }
  FloatType DistanceToPoint(const VecType& point) const {
    if (IsZeroLength()) {
      return (midpoint() - point).Length();
    }
    if ((end_ - start_).InnerProd(point - start_) < 0) {
      return (point - start_).Length();
    }
    if ((start_ - end_).InnerProd(point - end_) < 0) {
      return (point - end_).Length();
    }
    return std::abs((end_ - start_).CrossProd(point - start_)) / Length();
  }

  bool HasIntersectWithSegment(const Segment2& other_segment) const {
    if (min_x() > other_segment.max_x() || max_x() < other_segment.min_x() ||
        min_y() > other_segment.max_y() || max_y() < other_segment.min_y()) {
      return false;
    }
    FloatType c1 = (end_ - start_).CrossProd(other_segment.start() - start_);
    FloatType c2 = (end_ - start_).CrossProd(other_segment.end() - start_);
    if (c1 * c2 > 0) {
      return false;
    }
    FloatType c3 = (other_segment.end() - other_segment.start()).CrossProd(start_ - other_segment.start());
    FloatType c4 = (other_segment.end() - other_segment.start()).CrossProd(end_ - other_segment.start());
    if (c3 * c4 > 0) {
      return false;
    }
    return true;
  }
};

using Segment2d = Segment2<double>;
