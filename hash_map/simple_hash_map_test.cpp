#include "simple_hash_map.h"
#include "unordered_dense.h"
#include "tsl/robin_map.h"
#include "tsl/hopscotch_map.h"
// #include "parallel_hashmap/phmap.h"
#include "gtl/phmap.hpp"
#include "sparsepp/spp.h"

#include "gtest/gtest.h"

#include <random>

template <typename HashMap>
void TestHashMap(HashMap &map) {
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

  std::vector<unsigned long long> keys;
  std::vector<unsigned long long> keys_shuffled;
  constexpr int kNumKeys = 1000000;
  std::mt19937 generator;
  for (int i = 0; i < kNumKeys; ++i) {
    keys.push_back(generator());
  }
  keys_shuffled = keys;
  std::random_shuffle(keys_shuffled.begin(), keys_shuffled.end());

  for (auto key : keys) {
    map[key] = static_cast<int>(key);
  }
  for (auto key : keys_shuffled) {
    EXPECT_TRUE(map.count(key));
    EXPECT_EQ(map[key], static_cast<int>(key));
  }
}

old_hash_map my_old_hash_map;

TEST(old_hash_map, old_hash_map) {
  TestHashMap(my_old_hash_map);
}

new_hash_map my_new_hash_map;

TEST(new_hash_map, new_hash_map) {
  TestHashMap(my_new_hash_map);
}

/*
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
*/

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

/*
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
*/

int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::FLAGS_gtest_color = "yes";
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
