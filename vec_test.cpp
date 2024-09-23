#include "vec.h"

#include "gtest/gtest.h"

TEST(Vec3fTest, Add) {
  Vec3f a(1, 2, 3);
  Vec3f b(4, 5, 6);
  Vec3f result = a + b;
  EXPECT_NEAR(result.x, 5, 1e-9);
  EXPECT_NEAR(result.y, 7, 1e-9);
  EXPECT_NEAR(result.z, 9, 1e-9);
  a += b;
  EXPECT_NEAR(a.x, 5, 1e-9);
  EXPECT_NEAR(a.y, 7, 1e-9);
  EXPECT_NEAR(a.z, 9, 1e-9);
  a += a;
  EXPECT_NEAR(a.x, 10, 1e-9);
  EXPECT_NEAR(a.y, 14, 1e-9);
  EXPECT_NEAR(a.z, 18, 1e-9);
}

TEST(Vec3fTest, Sub) {
  Vec3f a(6, 5, 4);
  Vec3f b(1, 2, 3);
  Vec3f result = a - b;
  EXPECT_NEAR(result.x, 5, 1e-9);
  EXPECT_NEAR(result.y, 3, 1e-9);
  EXPECT_NEAR(result.z, 1, 1e-9);
  a -= b;
  EXPECT_NEAR(a.x, 5, 1e-9);
  EXPECT_NEAR(a.y, 3, 1e-9);
  EXPECT_NEAR(a.z, 1, 1e-9);
  a -= a;
  EXPECT_EQ(a.x, 0);
  EXPECT_EQ(a.y, 0);
  EXPECT_EQ(a.z, 0);
}

TEST(Vec3fTest, Mul) {
  Vec3f a(1, 2, 3);
  {
    Vec3f result = a * 1.5;
    EXPECT_NEAR(result.x, 1.5, 1e-9);
    EXPECT_NEAR(result.y, 3.0, 1e-9);
    EXPECT_NEAR(result.z, 4.5, 1e-9);
  }
  {
    Vec3f result = 1.5 * a;
    EXPECT_NEAR(result.x, 1.5, 1e-9);
    EXPECT_NEAR(result.y, 3.0, 1e-9);
    EXPECT_NEAR(result.z, 4.5, 1e-9);
  }
  a *= 1.5;
  EXPECT_NEAR(a.x, 1.5, 1e-9);
  EXPECT_NEAR(a.y, 3.0, 1e-9);
  EXPECT_NEAR(a.z, 4.5, 1e-9);
}

TEST(Vec3fTest, Div) {
  Vec3f a(1, 2, 3);
  Vec3f result = a / 2;
  EXPECT_NEAR(result.x, 0.5, 1e-9);
  EXPECT_NEAR(result.y, 1.0, 1e-9);
  EXPECT_NEAR(result.z, 1.5, 1e-9);
  a /= 2;
  EXPECT_NEAR(a.x, 0.5, 1e-9);
  EXPECT_NEAR(a.y, 1.0, 1e-9);
  EXPECT_NEAR(a.z, 1.5, 1e-9);
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
  EXPECT_EQ(a.x, na.x);
  EXPECT_EQ(a.y, na.y);
  EXPECT_EQ(a.z, na.z);
  EXPECT_EQ(b.x, nb.x);
  EXPECT_EQ(b.y, nb.y);
  EXPECT_EQ(b.z, nb.z);
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
  EXPECT_NEAR(ab.x, 0, 1e-9);
  EXPECT_NEAR(ab.y, 0, 1e-9);
  EXPECT_NEAR(ab.z, 6, 1e-9);

  Vec3f ba = b.CrossProd(a);
  EXPECT_NEAR(ba.x, 0, 1e-9);
  EXPECT_NEAR(ba.y, 0, 1e-9);
  EXPECT_NEAR(ba.z, -6, 1e-9);

  Vec3f ca = c.CrossProd(a);
  EXPECT_NEAR(ca.x, 0, 1e-9);
  EXPECT_NEAR(ca.y, 8, 1e-9);
  EXPECT_NEAR(ca.z, 0, 1e-9);

  Vec3f ac = a.CrossProd(c);
  EXPECT_NEAR(ac.x, 0, 1e-9);
  EXPECT_NEAR(ac.y, -8, 1e-9);
  EXPECT_NEAR(ac.z, 0, 1e-9);

  Vec3f bc = b.CrossProd(c);
  EXPECT_NEAR(bc.x, 12, 1e-9);
  EXPECT_NEAR(bc.y, 0, 1e-9);
  EXPECT_NEAR(bc.z, 0, 1e-9);

  Vec3f cb = c.CrossProd(b);
  EXPECT_NEAR(cb.x, -12, 1e-9);
  EXPECT_NEAR(cb.y, 0, 1e-9);
  EXPECT_NEAR(cb.z, 0, 1e-9);
}

int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::FLAGS_gtest_color = "yes";
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
