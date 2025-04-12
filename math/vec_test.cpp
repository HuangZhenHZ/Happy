#include "vec.h"

#include "gtest/gtest.h"

#define EXPECT_VEC3_NEAR(vec3, expect_x, expect_y, expect_z, error) \
  EXPECT_NEAR(vec3.x, expect_x, error); \
  EXPECT_NEAR(vec3.y, expect_y, error); \
  EXPECT_NEAR(vec3.z, expect_z, error);

#define EXPECT_VEC3_EQ(vec3, expect_x, expect_y, expect_z) \
  EXPECT_EQ(vec3.x, expect_x); \
  EXPECT_EQ(vec3.y, expect_y); \
  EXPECT_EQ(vec3.z, expect_z);

TEST(Vec3Test, Add) {
  Vec3f a(1, 2, 3);
  Vec3f b(4, 5, 6);
  Vec3f result = a + b;
  EXPECT_VEC3_NEAR(result, 5, 7, 9, 1e-9);
  a += b;
  EXPECT_VEC3_NEAR(a, 5, 7, 9, 1e-9);
  a += a;
  EXPECT_VEC3_NEAR(a, 10, 14, 18, 1e-9);
}

TEST(Vec3Test, Sub) {
  Vec3f a(6, 5, 4);
  Vec3f b(1, 2, 3);
  Vec3f result = a - b;
  EXPECT_VEC3_NEAR(result, 5, 3, 1, 1e-9);
  Vec3f c = -a;
  EXPECT_VEC3_NEAR(c, -6, -5, -4, 1e-9);
  a -= b;
  EXPECT_VEC3_NEAR(a, 5, 3, 1, 1e-9);
  a -= a;
  EXPECT_VEC3_EQ(a, 0, 0, 0);
}

TEST(Vec3Test, Mul) {
  Vec3f a(1, 2, 3);
  {
    Vec3f result = a * 1.5;
    EXPECT_VEC3_NEAR(result, 1.5, 3.0, 4.5, 1e-9);
  }
  {
    Vec3f result = 1.5 * a;
    EXPECT_VEC3_NEAR(result, 1.5, 3.0, 4.5, 1e-9);
  }
  a *= 1.5;
  EXPECT_VEC3_NEAR(a, 1.5, 3.0, 4.5, 1e-9);
}

TEST(Vec3Test, Div) {
  Vec3f a(1, 2, 3);
  Vec3f result = a / 2;
  EXPECT_VEC3_NEAR(result, 0.5, 1.0, 1.5, 1e-9);
  a /= 2;
  EXPECT_VEC3_NEAR(a, 0.5, 1.0, 1.5, 1e-9);
}

TEST(Vec3Test, Length) {
  Vec3f a(1, 2, 3);
  Vec3f b(2, 3, 4);
  EXPECT_NEAR(a.LengthSqr(), 14, 1e-9);
  EXPECT_NEAR(b.LengthSqr(), 29, 1e-9);
  EXPECT_NEAR(a.Length(), std::sqrt(14), 1e-6);
  EXPECT_NEAR(b.Length(), std::sqrt(29), 1e-6);
}

TEST(Vec3Test, Normalize) {
  Vec3f a(1, 2, 3);
  Vec3f b(2, 3, 4);
  Vec3f na = a.Normalized();
  Vec3f nb = b.Normalized();
  EXPECT_NEAR(na.LengthSqr(), 1, 1e-6);
  EXPECT_NEAR(nb.LengthSqr(), 1, 1e-6);
  EXPECT_NEAR(na.Length(), 1, 1e-6);
  EXPECT_NEAR(nb.Length(), 1, 1e-6);
  EXPECT_NEAR(na.x, a.x / a.Length(), 1e-6);
  EXPECT_NEAR(na.y, a.y / a.Length(), 1e-6);
  EXPECT_NEAR(na.z, a.z / a.Length(), 1e-6);
  EXPECT_NEAR(nb.x, b.x / b.Length(), 1e-6);
  EXPECT_NEAR(nb.y, b.y / b.Length(), 1e-6);
  EXPECT_NEAR(nb.z, b.z / b.Length(), 1e-6);
  a.Normalize();
  b.Normalize();
  EXPECT_VEC3_EQ(a, na.x, na.y, na.z);
  EXPECT_VEC3_EQ(b, nb.x, nb.y, nb.z);
}

TEST(Vec3Test, InnerProd) {
  Vec3f a(1, 2, 3);
  Vec3f b(2, 3, 4);
  EXPECT_NEAR(a.InnerProd(b), 20, 1e-9);
  EXPECT_NEAR(b.InnerProd(a), 20, 1e-9);
  EXPECT_EQ(a.InnerProd(a), a.LengthSqr());
  EXPECT_EQ(b.InnerProd(b), b.LengthSqr());
}

TEST(Vec3Test, CrossProd) {
  Vec3f a(2, 0, 0);
  Vec3f b(0, 3, 0);
  Vec3f c(0, 0, 4);
  Vec3f ab = a.CrossProd(b);
  Vec3f ba = b.CrossProd(a);
  Vec3f ca = c.CrossProd(a);
  Vec3f ac = a.CrossProd(c);
  Vec3f bc = b.CrossProd(c);
  Vec3f cb = c.CrossProd(b);
  EXPECT_VEC3_NEAR(ab, 0, 0, 6, 1e-9);
  EXPECT_VEC3_NEAR(ba, 0, 0, -6, 1e-9);
  EXPECT_VEC3_NEAR(ca, 0, 8, 0, 1e-9);
  EXPECT_VEC3_NEAR(ac, 0, -8, 0, 1e-9);
  EXPECT_VEC3_NEAR(bc, 12, 0, 0, 1e-9);
  EXPECT_VEC3_NEAR(cb, -12, 0, 0, 1e-9);
}

TEST(Vec2Test, Add) {
  Vec2f a(1, 2);
  Vec2f b(3, 4);
  Vec2f result = a + b;
  EXPECT_NEAR(result.x, 4, 1e-9);
  EXPECT_NEAR(result.y, 6, 1e-9);
  a += b;
  EXPECT_NEAR(a.x, 4, 1e-9);
  EXPECT_NEAR(a.y, 6, 1e-9);
  a += a;
  EXPECT_NEAR(a.x, 8, 1e-9);
  EXPECT_NEAR(a.y, 12, 1e-9);
}

TEST(Vec2Test, Sub) {
  Vec2f a(4, 3);
  Vec2f b(1, 2);
  Vec2f result = a - b;
  EXPECT_NEAR(result.x, 3, 1e-9);
  EXPECT_NEAR(result.y, 1, 1e-9);
  Vec2f c = -a;
  EXPECT_NEAR(c.x, -4, 1e-9);
  EXPECT_NEAR(c.y, -3, 1e-9);
  a -= b;
  EXPECT_NEAR(a.x, 3, 1e-9);
  EXPECT_NEAR(a.y, 1, 1e-9);
  a -= a;
  EXPECT_EQ(a.x, 0);
  EXPECT_EQ(a.y, 0);
}

TEST(Vec2Test, Mul) {
  Vec2f a(1, 2);
  {
    Vec2f result = a * 1.5;
    EXPECT_NEAR(result.x, 1.5, 1e-9);
    EXPECT_NEAR(result.y, 3.0, 1e-9);
  }
  {
    Vec2f result = 1.5 * a;
    EXPECT_NEAR(result.x, 1.5, 1e-9);
    EXPECT_NEAR(result.y, 3.0, 1e-9);
  }
  a *= 1.5;
  EXPECT_NEAR(a.x, 1.5, 1e-9);
  EXPECT_NEAR(a.y, 3.0, 1e-9);
}

TEST(Vec2Test, Div) {
  Vec2f a(1, 2);
  Vec2f result = a / 2;
  EXPECT_NEAR(result.x, 0.5, 1e-9);
  EXPECT_NEAR(result.y, 1.0, 1e-9);
  a /= 2;
  EXPECT_NEAR(a.x, 0.5, 1e-9);
  EXPECT_NEAR(a.y, 1.0, 1e-9);
}

TEST(Vec2Test, Basic) {
  const auto test = [](float length, float heading) {
    Vec2f a = length * Vec2f::FromUnit(heading);
    EXPECT_EQ(a.x, length * std::cos(heading));
    EXPECT_EQ(a.y, length * std::sin(heading));
    EXPECT_NEAR(a.Length(), std::abs(length), 1e-6);
    EXPECT_NEAR(a.LengthSqr(), length * length, 1e-6);
    double expected_angle = length > 0 ? NormalizeAngle(heading) :
        NormalizeAngle(heading + M_PI);
    EXPECT_NEAR(a.Angle(), expected_angle, 1e-6);
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
  Vec2f na = a.Normalized();
  Vec2f nb = b.Normalized();
  EXPECT_NEAR(na.LengthSqr(), 1, 1e-6);
  EXPECT_NEAR(nb.LengthSqr(), 1, 1e-6);
  EXPECT_NEAR(na.Length(), 1, 1e-6);
  EXPECT_NEAR(nb.Length(), 1, 1e-6);
  EXPECT_NEAR(na.x, a.x / a.Length(), 1e-6);
  EXPECT_NEAR(na.y, a.y / a.Length(), 1e-6);
  EXPECT_NEAR(nb.x, b.x / b.Length(), 1e-6);
  EXPECT_NEAR(nb.y, b.y / b.Length(), 1e-6);
  a.Normalize();
  b.Normalize();
  EXPECT_EQ(a.x, na.x);
  EXPECT_EQ(a.y, na.y);
  EXPECT_EQ(b.x, nb.x);
  EXPECT_EQ(b.y, nb.y);
}

TEST(Vec2Test, InnerProdAndCrossProd) {
  Vec2f a(1, 2);
  Vec2f b(3, 4);
  EXPECT_NEAR(a.InnerProd(b), 11, 1e-9);
  EXPECT_NEAR(b.InnerProd(a), 11, 1e-9);
  EXPECT_EQ(a.InnerProd(a), a.LengthSqr());
  EXPECT_EQ(b.InnerProd(b), b.LengthSqr());
  EXPECT_NEAR(a.CrossProd(b), -2, 1e-9);
  EXPECT_NEAR(b.CrossProd(a), 2, 1e-9);
  EXPECT_EQ(a.CrossProd(a), 0);
  EXPECT_EQ(b.CrossProd(b), 0);
  EXPECT_EQ(a.CrossProd(b), a.Rotated90().InnerProd(b));
  EXPECT_EQ(a.CrossProd(b), a.InnerProd(b.Rotated270()));
  EXPECT_EQ(b.CrossProd(a), b.Rotated90().InnerProd(a));
  EXPECT_EQ(b.CrossProd(a), b.InnerProd(a.Rotated270()));
}

TEST(Vec2Test, Rotate) {
  const Vec2f vecs[] = {
    3.0 * Vec2f::FromUnit(1.23),
    3.0 * Vec2f::FromUnit(1.23 + M_PI_2),
    3.0 * Vec2f::FromUnit(1.23 + M_PI),
    3.0 * Vec2f::FromUnit(1.23 - M_PI_2),
  };
  for (int i = 0; i < 4; ++i) {
    Vec2f rotate90 = vecs[i].Rotated90();
    EXPECT_NEAR(rotate90.x, vecs[(i + 1) % 4].x, 1e-6);
    EXPECT_NEAR(rotate90.y, vecs[(i + 1) % 4].y, 1e-6);

    Vec2f rotate180 = vecs[i].Rotated180();
    EXPECT_NEAR(rotate180.x, vecs[(i + 2) % 4].x, 1e-6);
    EXPECT_NEAR(rotate180.y, vecs[(i + 2) % 4].y, 1e-6);

    Vec2f rotate270 = vecs[i].Rotated270();
    EXPECT_NEAR(rotate270.x, vecs[(i + 3) % 4].x, 1e-6);
    EXPECT_NEAR(rotate270.y, vecs[(i + 3) % 4].y, 1e-6);

    Vec2f a = vecs[i];
    a.Rotate90();
    EXPECT_NEAR(a.x, vecs[(i + 1) % 4].x, 1e-6);
    EXPECT_NEAR(a.y, vecs[(i + 1) % 4].y, 1e-6);
    a.Rotate180();
    EXPECT_NEAR(a.x, vecs[(i + 3) % 4].x, 1e-6);
    EXPECT_NEAR(a.y, vecs[(i + 3) % 4].y, 1e-6);
    a.Rotate270();
    EXPECT_NEAR(a.x, vecs[(i + 2) % 4].x, 1e-6);
    EXPECT_NEAR(a.y, vecs[(i + 2) % 4].y, 1e-6);
  }

  const Vec2f a = 3.0 * Vec2f::FromUnit(1.23);
  const Vec2f b = a.RotatedByUnit(std::cos(0.45), std::sin(0.45));
  const Vec2f c = 3.0 * Vec2f::FromUnit(1.23 + 0.45);
  EXPECT_NEAR(b.x, c.x, 1e-6);
  EXPECT_NEAR(b.y, c.y, 1e-6);
  Vec2f d = a;
  d.RotateByUnit(std::cos(0.45), std::sin(0.45));
  EXPECT_NEAR(d.x, c.x, 1e-6);
  EXPECT_NEAR(d.y, c.y, 1e-6);
}

TEST(MatrixTest, Add) {
  Matrix<double, 2, 3> a;
  a(0, 0) = 1; a(0, 1) = 2; a(0, 2) = 3;
  a(1, 0) = 4; a(1, 1) = 5; a(1, 2) = 6;

  Matrix<double, 2, 3> b;
  b(0, 0) = 2; b(0, 1) = 3; b(0, 2) = 3;
  b(1, 0) = 6; b(1, 1) = 6; b(1, 2) = 6;

  Matrix<double, 2, 3> c = a + b;
  EXPECT_NEAR(c(0, 0), 3, 1e-9);
  EXPECT_NEAR(c(0, 1), 5, 1e-9);
  EXPECT_NEAR(c(0, 2), 6, 1e-9);
  EXPECT_NEAR(c(1, 0), 10, 1e-9);
  EXPECT_NEAR(c(1, 1), 11, 1e-9);
  EXPECT_NEAR(c(1, 2), 12, 1e-9);
}

TEST(MatrixTest, Sub) {
  Matrix<double, 2, 3> a;
  a(0, 0) = 1; a(0, 1) = 2; a(0, 2) = 3;
  a(1, 0) = 4; a(1, 1) = 5; a(1, 2) = 6;

  Matrix<double, 2, 3> b;
  b(0, 0) = 2; b(0, 1) = 3; b(0, 2) = 3;
  b(1, 0) = 6; b(1, 1) = 6; b(1, 2) = 6;

  Matrix<double, 2, 3> c = a - b;
  EXPECT_NEAR(c(0, 0), -1, 1e-9);
  EXPECT_NEAR(c(0, 1), -1, 1e-9);
  EXPECT_NEAR(c(0, 2), 0, 1e-9);
  EXPECT_NEAR(c(1, 0), -2, 1e-9);
  EXPECT_NEAR(c(1, 1), -1, 1e-9);
  EXPECT_NEAR(c(1, 2), 0, 1e-9);
}

TEST(MatrixTest, Mul) {
  Mat4f a;
  a(0, 0) = 1; a(0, 1) = 2;
  a(1, 0) = 3; a(1, 1) = 4;

  Mat4f b;
  b(0, 0) = 1; b(0, 1) = 2;
  b(1, 0) = 3; b(1, 1) = 4;

  Mat4f c = a * b;
  EXPECT_NEAR(c(0, 0), 7, 1e-9);
  EXPECT_NEAR(c(0, 1), 10, 1e-9);
  EXPECT_NEAR(c(1, 0), 15, 1e-9);
  EXPECT_NEAR(c(1, 1), 22, 1e-9);
}

int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::FLAGS_gtest_color = "yes";
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
