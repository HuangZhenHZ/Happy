#include "segment2d.h"

#include "gtest/gtest.h"

TEST(Segment2dTest, Basic) {
  Segment2d segment(Vec2d(1, 2), Vec2d(3, 4));
  EXPECT_EQ(segment.start().x(), 1);
  EXPECT_EQ(segment.start().y(), 2);
  EXPECT_EQ(segment.end().x(), 3);
  EXPECT_EQ(segment.end().y(), 4);
  EXPECT_EQ(segment.min_x(), 1);
  EXPECT_EQ(segment.max_x(), 3);
  EXPECT_EQ(segment.min_y(), 2);
  EXPECT_EQ(segment.max_y(), 4);
  EXPECT_EQ(segment.LengthSqr(), 8);
  EXPECT_EQ(segment.Length(), std::sqrt(8));
}

TEST(Segment2dTest, DistanceToPoint) {
  {
    Segment2d segment(Vec2d(1, 0), Vec2d(3, 0));
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(0, 1)), std::sqrt(2));
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(0, -1)), std::sqrt(2));
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(4, 1)), std::sqrt(2));
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(4, -1)), std::sqrt(2));
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(-2, 4)), 5);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(-2, -4)), 5);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(6, 4)), 5);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(6, -4)), 5);

    EXPECT_EQ(segment.DistanceToPoint(Vec2d(1, 4)), 4);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(2, 4)), 4);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(3, 4)), 4);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(1, -4)), 4);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(2, -4)), 4);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(3, -4)), 4);
  }
  {
    Segment2d segment(Vec2d(0, 1), Vec2d(0, 3));
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(1, 0)), std::sqrt(2));
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(-1, 0)), std::sqrt(2));
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(1, 4)), std::sqrt(2));
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(-1, 4)), std::sqrt(2));
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(4, -2)), 5);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(-4, -2)), 5);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(4, 6)), 5);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(-4, 6)), 5);

    EXPECT_EQ(segment.DistanceToPoint(Vec2d(4, 1)), 4);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(4, 2)), 4);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(4, 3)), 4);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(-4, 1)), 4);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(-4, 2)), 4);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(-4, 3)), 4);
  }
  {
    Segment2d segment(Vec2d(2, 0), Vec2d(0, 2));
    EXPECT_NEAR(segment.DistanceToPoint(Vec2d(0, 0)), std::sqrt(2), 1e-9);
    EXPECT_NEAR(segment.DistanceToPoint(Vec2d(2, 2)), std::sqrt(2), 1e-9);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(0, 5)), 3);
    EXPECT_EQ(segment.DistanceToPoint(Vec2d(5, 0)), 3);
  }
}

TEST(Segment2dTest, HasIntersectWithSegment) {
  Segment2d segment1(Vec2d(-1, 0), Vec2d(1, 0));
  Segment2d segment2(Vec2d(0, -1), Vec2d(0, 1));
  for (int i = -10; i <= 10; ++i) {
    for (int j = -10; j <= 10; ++j) {
      EXPECT_EQ(segment1.HasIntersectWithSegment(Segment2d(Vec2d(i, -1), Vec2d(j, 1))),
                i + j >= -2 && i + j <= 2);
      EXPECT_EQ(Segment2d(Vec2d(i, -1), Vec2d(j, 1)).HasIntersectWithSegment(segment1),
                i + j >= -2 && i + j <= 2);
      EXPECT_EQ(segment2.HasIntersectWithSegment(Segment2d(Vec2d(-1, i), Vec2d(1, j))),
                i + j >= -2 && i + j <= 2);
      EXPECT_EQ(Segment2d(Vec2d(-1, i), Vec2d(1, j)).HasIntersectWithSegment(segment2),
                i + j >= -2 && i + j <= 2);
    }
  }
}

int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::FLAGS_gtest_color = "yes";
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
