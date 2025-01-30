#include "polygon2d.h"

#include "gtest/gtest.h"

TEST(Polygon2dTest, Basic) {
  Polygon2d polygon({Vec2d(1, 1), Vec2d(2, 1), Vec2d(1, 2)});
  EXPECT_NEAR(polygon.area(), 0.5, 1e-9);

  EXPECT_TRUE(polygon.IsPointIn(Vec2d(1.2, 1.2)));
  EXPECT_FALSE(polygon.IsPointIn(Vec2d(1.8, 1.8)));
  EXPECT_FALSE(polygon.IsPointIn(Vec2d(-0.2, 1.2)));

  for (const double l : {0.0, 1.0, 1.2}) {
    for (const double r : {1.3, 1.5, 2.0}) {
      EXPECT_TRUE(polygon.HasOverlapWithSegment(Segment2d(Vec2d(l, l), Vec2d(r, r))));
    }
  }
  EXPECT_FALSE(polygon.HasOverlapWithSegment(Segment2d(Vec2d(0, 0), Vec2d(0, 1.5))));
}

int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::FLAGS_gtest_color = "yes";
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
