#include "vec.h"

#include "gtest/gtest.h"

TEST(Vec2Test, Add) {
  Vec2f a(1, 2);
  Vec2f b(3, 4);
  Vec2f result = a + b;
  EXPECT_NEAR(result.x(), 4, 1e-9);
  EXPECT_NEAR(result.y(), 6, 1e-9);
  a += b;
  EXPECT_NEAR(a.x(), 4, 1e-9);
  EXPECT_NEAR(a.y(), 6, 1e-9);
  a += a;
  EXPECT_NEAR(a.x(), 8, 1e-9);
  EXPECT_NEAR(a.y(), 12, 1e-9);
}

TEST(Vec2Test, Sub) {
  Vec2f a(4, 3);
  Vec2f b(1, 2);
  Vec2f result = a - b;
  EXPECT_NEAR(result.x(), 3, 1e-9);
  EXPECT_NEAR(result.y(), 1, 1e-9);
  Vec2f c = -a;
  EXPECT_NEAR(c.x(), -4, 1e-9);
  EXPECT_NEAR(c.y(), -3, 1e-9);
  a -= b;
  EXPECT_NEAR(a.x(), 3, 1e-9);
  EXPECT_NEAR(a.y(), 1, 1e-9);
  a -= a;
  EXPECT_EQ(a.x(), 0);
  EXPECT_EQ(a.y(), 0);
}

TEST(Vec2Test, Mul) {
  Vec2f a(1, 2);
  {
    Vec2f result = a * 1.5;
    EXPECT_NEAR(result.x(), 1.5, 1e-9);
    EXPECT_NEAR(result.y(), 3.0, 1e-9);
  }
  {
    Vec2f result = 1.5 * a;
    EXPECT_NEAR(result.x(), 1.5, 1e-9);
    EXPECT_NEAR(result.y(), 3.0, 1e-9);
  }
  a *= 1.5;
  EXPECT_NEAR(a.x(), 1.5, 1e-9);
  EXPECT_NEAR(a.y(), 3.0, 1e-9);
}

TEST(Vec2Test, Div) {
  Vec2f a(1, 2);
  Vec2f result = a / 2;
  EXPECT_NEAR(result.x(), 0.5, 1e-9);
  EXPECT_NEAR(result.y(), 1.0, 1e-9);
  a /= 2;
  EXPECT_NEAR(a.x(), 0.5, 1e-9);
  EXPECT_NEAR(a.y(), 1.0, 1e-9);
}

TEST(Vec2Test, Basic) {
  const auto test = [](float length, float heading) {
    Vec2f a = length * FromUnit(heading);
    EXPECT_EQ(a.x(), length * std::cos(heading));
    EXPECT_EQ(a.y(), length * std::sin(heading));
    EXPECT_NEAR(Length(a), std::abs(length), 1e-6);
    EXPECT_NEAR(LengthSqr(a), length * length, 1e-6);
    double expected_angle = length > 0 ? NormalizeAngle(heading) :
        NormalizeAngle(heading + M_PI);
    EXPECT_NEAR(Angle(a), expected_angle, 1e-6);
  };
  test(1.0, 1.23);
  test(-1.0, 1.23);
  test(3.0, 0.6);
  test(-3.0, 0.6);
  test(5.5, 123);
  test(5.5, -123);
  test(-5.5, 123);
  test(-5.5, -123);
}

TEST(Vec2Test, Normalize) {
  Vec2f a(1, 2);
  Vec2f b(3, 4);
  Vec2f na = a.normalized();
  Vec2f nb = b.normalized();
  EXPECT_NEAR(LengthSqr(na), 1, 1e-6);
  EXPECT_NEAR(LengthSqr(nb), 1, 1e-6);
  EXPECT_NEAR(Length(na), 1, 1e-6);
  EXPECT_NEAR(Length(nb), 1, 1e-6);
  EXPECT_NEAR(na.x(), a.x() / Length(a), 1e-6);
  EXPECT_NEAR(na.y(), a.y() / Length(a), 1e-6);
  EXPECT_NEAR(nb.x(), b.x() / Length(b), 1e-6);
  EXPECT_NEAR(nb.y(), b.y() / Length(b), 1e-6);
  a.normalize();
  b.normalize();
  EXPECT_EQ(a.x(), na.x());
  EXPECT_EQ(a.y(), na.y());
  EXPECT_EQ(b.x(), nb.x());
  EXPECT_EQ(b.y(), nb.y());
}

TEST(Vec2Test, InnerProdAndCrossProd) {
  Vec2f a(1, 2);
  Vec2f b(3, 4);
  EXPECT_NEAR(InnerProd(a, b), 11, 1e-9);
  EXPECT_NEAR(InnerProd(b, a), 11, 1e-9);
  EXPECT_EQ(InnerProd(a, a), LengthSqr(a));
  EXPECT_EQ(InnerProd(b, b), LengthSqr(b));
  EXPECT_NEAR(CrossProd(a, b), -2, 1e-9);
  EXPECT_NEAR(CrossProd(b, a), 2, 1e-9);
  EXPECT_EQ(CrossProd(a, a), 0);
  EXPECT_EQ(CrossProd(b, b), 0);
  EXPECT_EQ(CrossProd(a, b), InnerProd(Rotated90(a), b));
  EXPECT_EQ(CrossProd(a, b), InnerProd(a, Rotated270(b)));
  EXPECT_EQ(CrossProd(b, a), InnerProd(Rotated90(b), a));
  EXPECT_EQ(CrossProd(b, a), InnerProd(b, Rotated270(a)));
}

TEST(Vec2Test, Rotate) {
  const Vec2f vecs[] = {
    3.0 * FromUnit<float>(1.23),
    3.0 * FromUnit<float>(1.23 + M_PI_2),
    3.0 * FromUnit<float>(1.23 + M_PI),
    3.0 * FromUnit<float>(1.23 - M_PI_2),
  };
  for (int i = 0; i < 4; ++i) {
    Vec2f rotate90 = Rotated90(vecs[i]);
    EXPECT_NEAR(rotate90.x(), vecs[(i + 1) % 4].x(), 1e-6);
    EXPECT_NEAR(rotate90.y(), vecs[(i + 1) % 4].y(), 1e-6);

    Vec2f rotate180 = Rotated180(vecs[i]);
    EXPECT_NEAR(rotate180.x(), vecs[(i + 2) % 4].x(), 1e-6);
    EXPECT_NEAR(rotate180.y(), vecs[(i + 2) % 4].y(), 1e-6);

    Vec2f rotate270 = Rotated270(vecs[i]);
    EXPECT_NEAR(rotate270.x(), vecs[(i + 3) % 4].x(), 1e-6);
    EXPECT_NEAR(rotate270.y(), vecs[(i + 3) % 4].y(), 1e-6);

    Vec2f a = vecs[i];
    Rotate90(&a);
    EXPECT_NEAR(a.x(), vecs[(i + 1) % 4].x(), 1e-6);
    EXPECT_NEAR(a.y(), vecs[(i + 1) % 4].y(), 1e-6);
    Rotate180(&a);
    EXPECT_NEAR(a.x(), vecs[(i + 3) % 4].x(), 1e-6);
    EXPECT_NEAR(a.y(), vecs[(i + 3) % 4].y(), 1e-6);
    Rotate270(&a);
    EXPECT_NEAR(a.x(), vecs[(i + 2) % 4].x(), 1e-6);
    EXPECT_NEAR(a.y(), vecs[(i + 2) % 4].y(), 1e-6);
  }

  const Vec2f a = 3.0 * FromUnit<float>(1.23);
  const Vec2f b = RotatedByUnit(a, std::cos(0.45), std::sin(0.45));
  const Vec2f c = 3.0 * FromUnit<float>(1.23 + 0.45);
  EXPECT_NEAR(b.x(), c.x(), 1e-6);
  EXPECT_NEAR(b.y(), c.y(), 1e-6);

  Vec2f d = a;
  RotateByUnit(&d, std::cos(0.45), std::sin(0.45));
  EXPECT_NEAR(d.x(), c.x(), 1e-6);
  EXPECT_NEAR(d.y(), c.y(), 1e-6);
}

int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::FLAGS_gtest_color = "yes";
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
