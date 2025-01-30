#include "box2d.h"

#include "gtest/gtest.h"

TEST(Box2dTest, Basic) {
  for (const double cos_heading : {-0.8, 0.8}) {
    for (const double sin_heading : {-0.6, 0.6}) {
      Box2d box({1.0, 2.0}, {cos_heading, sin_heading}, 8, 6);
      EXPECT_EQ(box.center().x, 1);
      EXPECT_EQ(box.center().y, 2);
      EXPECT_EQ(box.heading().x, cos_heading);
      EXPECT_EQ(box.heading().y, sin_heading);
      EXPECT_EQ(box.length(), 8);
      EXPECT_EQ(box.width(), 6);
      EXPECT_EQ(box.half_length(), 4);
      EXPECT_EQ(box.half_width(), 3);
      EXPECT_NEAR(box.min_x(), 1.0 - 0.8 * 4 - 0.6 * 3, 1e-9);
      EXPECT_NEAR(box.max_x(), 1.0 + 0.8 * 4 + 0.6 * 3, 1e-9);
      EXPECT_NEAR(box.min_y(), 2.0 - 0.8 * 3 - 0.6 * 4, 1e-9);
      EXPECT_NEAR(box.max_y(), 2.0 + 0.8 * 3 + 0.6 * 4, 1e-9);

      std::array<Vec2d, 4> corners = box.GetCornersArray();
      double min_x = std::numeric_limits<double>::infinity();
      double max_x = -std::numeric_limits<double>::infinity();
      double min_y = std::numeric_limits<double>::infinity();
      double max_y = -std::numeric_limits<double>::infinity();
      for (const Vec2d& corner : corners) {
        min_x = std::min(min_x, corner.x);
        max_x = std::max(max_x, corner.x);
        min_y = std::min(min_y, corner.y);
        max_y = std::max(max_y, corner.y);
      }
      EXPECT_NEAR(box.min_x(), min_x, 1e-9);
      EXPECT_NEAR(box.max_x(), max_x, 1e-9);
      EXPECT_NEAR(box.min_y(), min_y, 1e-9);
      EXPECT_NEAR(box.max_y(), max_y, 1e-9);
    }
  }
}

TEST(Box2dTest, DistanceToPoint) {
  const auto compute_distance = [](const Box2d& box, const Vec2d& point) {
    std::array<Vec2d, 4> corners = box.GetCornersArray();
    double double_area =
        std::abs((corners[0] - point).CrossProd(corners[1] - point)) +
        std::abs((corners[1] - point).CrossProd(corners[2] - point)) +
        std::abs((corners[2] - point).CrossProd(corners[3] - point)) +
        std::abs((corners[3] - point).CrossProd(corners[0] - point));
    if (double_area <= box.area() * 2 + 1e-9) {
      return 0.0;
    }
    return std::min({
        Segment2d(corners[0], corners[1]).DistanceToPoint(point),
        Segment2d(corners[1], corners[2]).DistanceToPoint(point),
        Segment2d(corners[2], corners[3]).DistanceToPoint(point),
        Segment2d(corners[3], corners[0]).DistanceToPoint(point),
    });
  };

  {
    Box2d box({4.0, 3.0}, {1.0, 0.0}, 8, 6);
    for (int x = -2; x <= 2; ++x) {
      for (int y = -2; y <= 2; ++y) {
        double distance = ApproxHypot(double(std::max(-x, 0)), double(std::max(-y, 0)));
        EXPECT_NEAR(box.DistanceToPoint(Vec2d(x, y)), distance, 1e-9);
        EXPECT_NEAR(compute_distance(box, Vec2d(x, y)), distance, 1e-9);
      }
    }
  }

  for (const double cos_heading : {-0.8, 0.8}) {
    for (const double sin_heading : {-0.6, 0.6}) {
      Box2d box({1.0, 2.0}, {cos_heading, sin_heading}, 4, 3);
      for (int x = 0; x < 10; ++x) {
        for (int y = 0; y < 10; ++y) {
          Vec2d point(x, y);
          EXPECT_NEAR(box.DistanceToPoint(point), compute_distance(box, point), 1e-9);
        }
      }
    }
  }
}

int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::FLAGS_gtest_color = "yes";
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
