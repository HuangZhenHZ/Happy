#include "simple_hash_map.h"
#include "unordered_dense.h"
#include "tsl/robin_map.h"
#include "tsl/hopscotch_map.h"
// #include "parallel_hashmap/phmap.h"
#include "gtl/phmap.hpp"
#include "sparsepp/spp.h"

#include "gtest/gtest.h"

old_hash_map my_old_hash_map;

TEST(old_hash_map, old_hash_map_test) {
  EXPECT_FALSE(my_old_hash_map.count(0));
  EXPECT_FALSE(my_old_hash_map.count(1));
  my_old_hash_map[0] = 12;
  my_old_hash_map[1] = 34;
  EXPECT_TRUE(my_old_hash_map.count(0));
  EXPECT_TRUE(my_old_hash_map.count(1));
  EXPECT_FALSE(my_old_hash_map.count(2));
  EXPECT_EQ(my_old_hash_map[0], 12);
  EXPECT_EQ(my_old_hash_map[1], 34);
  EXPECT_EQ(my_old_hash_map[2], 0);
  EXPECT_TRUE(my_old_hash_map.count(2));
}

TEST(ankerl, ankerl) {
  ankerl::unordered_dense::map<unsigned long long, int> ankerl_map;
  EXPECT_FALSE(ankerl_map.count(0));
  EXPECT_FALSE(ankerl_map.count(1));
  ankerl_map[0] = 12;
  ankerl_map[1] = 34;
  EXPECT_TRUE(ankerl_map.count(0));
  EXPECT_TRUE(ankerl_map.count(1));
  EXPECT_FALSE(ankerl_map.count(2));
  EXPECT_EQ(ankerl_map[0], 12);
  EXPECT_EQ(ankerl_map[1], 34);
  EXPECT_EQ(ankerl_map[2], 0);
  EXPECT_TRUE(ankerl_map.count(2));
}

TEST(tsl, robin_map) {
  tsl::robin_map<unsigned long long, int> tsl_map;
  EXPECT_FALSE(tsl_map.count(0));
  EXPECT_FALSE(tsl_map.count(1));
  tsl_map[0] = 12;
  tsl_map[1] = 34;
  EXPECT_TRUE(tsl_map.count(0));
  EXPECT_TRUE(tsl_map.count(1));
  EXPECT_FALSE(tsl_map.count(2));
  EXPECT_EQ(tsl_map[0], 12);
  EXPECT_EQ(tsl_map[1], 34);
  EXPECT_EQ(tsl_map[2], 0);
  EXPECT_TRUE(tsl_map.count(2));
}

TEST(tsl, hopscotch_map) {
  tsl::hopscotch_map<unsigned long long, int> tsl_map;
  EXPECT_FALSE(tsl_map.count(0));
  EXPECT_FALSE(tsl_map.count(1));
  tsl_map[0] = 12;
  tsl_map[1] = 34;
  EXPECT_TRUE(tsl_map.count(0));
  EXPECT_TRUE(tsl_map.count(1));
  EXPECT_FALSE(tsl_map.count(2));
  EXPECT_EQ(tsl_map[0], 12);
  EXPECT_EQ(tsl_map[1], 34);
  EXPECT_EQ(tsl_map[2], 0);
  EXPECT_TRUE(tsl_map.count(2));
}

/*
TEST(phmap, phmap) {
  phmap::flat_hash_map<unsigned long long, int> map;
  EXPECT_FALSE(map.count(0));
  EXPECT_FALSE(map.count(1));
  map[0] = 12;
  map[1] = 34;
  EXPECT_TRUE(map.count(0));
  EXPECT_TRUE(map.count(1));
  EXPECT_FALSE(map.count(2));
  EXPECT_EQ(map[0], 12);
  EXPECT_EQ(map[1], 34);
  EXPECT_EQ(map[2], 0);
  EXPECT_TRUE(map.count(2));
}
*/

TEST(gtlmap, gtlmap) {
  gtl::flat_hash_map<unsigned long long, int> map;
  EXPECT_FALSE(map.count(0));
  EXPECT_FALSE(map.count(1));
  map[0] = 12;
  map[1] = 34;
  EXPECT_TRUE(map.count(0));
  EXPECT_TRUE(map.count(1));
  EXPECT_FALSE(map.count(2));
  EXPECT_EQ(map[0], 12);
  EXPECT_EQ(map[1], 34);
  EXPECT_EQ(map[2], 0);
  EXPECT_TRUE(map.count(2));
}

TEST(sppmap, sppmap) {
  spp::sparse_hash_map<unsigned long long, int> map;
  EXPECT_FALSE(map.count(0));
  EXPECT_FALSE(map.count(1));
  map[0] = 12;
  map[1] = 34;
  EXPECT_TRUE(map.count(0));
  EXPECT_TRUE(map.count(1));
  EXPECT_FALSE(map.count(2));
  EXPECT_EQ(map[0], 12);
  EXPECT_EQ(map[1], 34);
  EXPECT_EQ(map[2], 0);
  EXPECT_TRUE(map.count(2));
}

int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::FLAGS_gtest_color = "yes";
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
