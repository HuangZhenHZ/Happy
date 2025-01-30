#pragma once

#include "segment2d.h"
#include "vec.h"

#include <vector>

template <typename FloatType>
class Polygon2 {
  using VecType = Vec2<FloatType>;
  using SegmentType = Segment2<FloatType>;
  std::vector<VecType> points_;
public:
  Polygon2() = default;
  explicit Polygon2(std::vector<VecType> points) : points_(std::move(points)) {}

  FloatType area() const {
    if (points_.size() < 3) {
      return 0;
    }
    FloatType result = points_.back().CrossProd(points_.front());
    for (int i = 0; i + 1 < (int)points_.size(); ++i) {
      result += points_[i].CrossProd(points_[i + 1]);
    }
    return result * 0.5;
  }

  bool IsPointIn(const VecType& point) const {
    if (points_.empty()) {
      return false;
    }
    bool result = false;
    VecType last_point = points_.back();
    for (const VecType& this_point : points_) {
      if ((this_point.y > point.y) ^ (last_point.y > point.y)) {
        const FloatType cross = (this_point - last_point).CrossProd(point - last_point);
        result ^= (last_point.y > this_point.y) ^ (cross > 0);
      }
      last_point = this_point;
    }
    return result;
  }

  bool HasOverlapWithSegment(const SegmentType& segment) {
    if (IsPointIn(segment.start()) || IsPointIn(segment.end())) {
      return true;
    }
    VecType last_point = points_.back();
    for (const VecType& point : points_) {
      if (segment.HasIntersectWithSegment(SegmentType(last_point, point))) {
        return true;
      }
      last_point = point;
    }
    return false;
  }
};

using Polygon2d = Polygon2<double>;
