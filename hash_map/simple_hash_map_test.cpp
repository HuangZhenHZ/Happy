#include "simple_hash_map.h"
#include "unordered_dense.h"
#include "tsl/robin_map.h"
#include "tsl/hopscotch_map.h"
// #include "parallel_hashmap/phmap.h"
#include "gtl/phmap.hpp"
#include "sparsepp/spp.h"
// #include "absl/container/flat_hash_map.h"
#include "hash_table8.hpp"

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

TEST(vector_hash_map, vector_hash_map) {
  VectorHashMap vector_hash_map;
  TestHashMap(vector_hash_map);
}

// TEST(FlatHashMap, FlatHashMap) {
//   FlatHashMap flat_hash_map;
//   TestHashMap(flat_hash_map);
//   flat_hash_map.PrintDebugInfo();
//   // flat_hash_map[1ll << 60] = 1;
//   // flat_hash_map.PrintDebugInfo();
//   // flat_hash_map[2ll << 60] = 2;
//   // EXPECT_TRUE(flat_hash_map.count(2ll << 60));
//   // flat_hash_map.PrintDebugInfo();
//   // flat_hash_map[1ll << 60 | 1] = 3;
//   // flat_hash_map.PrintDebugInfo();
//   // EXPECT_TRUE(flat_hash_map.count(1ll << 60));
//   // EXPECT_TRUE(flat_hash_map.count(2ll << 60));
//   // EXPECT_TRUE(flat_hash_map.count(1ll << 60 | 1));
// }

TEST(HybridHashMap, HybridHashMap) {
  HybridHashMap map;
  TestHashMap(map);
}

TEST(ankerl, ankerl) {
  ankerl::unordered_dense::map<unsigned long long, int> ankerl_map;
  TestHashMap(ankerl_map);
}

TEST(tsl, robin_map) {
  tsl::robin_map<unsigned long long, int> tsl_map;
  TestHashMap(tsl_map);
}

TEST(tsl, hopscotch_map) {
  tsl::hopscotch_map<unsigned long long, int> tsl_map;
  TestHashMap(tsl_map);
}

TEST(gtlmap, gtlmap) {
  gtl::flat_hash_map<unsigned long long, int> map;
  TestHashMap(map);
}

TEST(sppmap, sppmap) {
  spp::sparse_hash_map<unsigned long long, int> map;
  TestHashMap(map);
}

TEST(emhash8, emhash8) {
  emhash8::HashMap<unsigned long long, int> map;
  TestHashMap(map);
}

// TEST(googlemap, googlemap) {
//   absl::flat_hash_map<unsigned long long, int> map;
//   TestHashMap(map);
// }

int main(int argc, char **argv) {
  printf("Running main() from %s\n", __FILE__);
  testing::FLAGS_gtest_color = "yes";
  testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
