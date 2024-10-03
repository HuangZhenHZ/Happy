#include "simple_hash_map.h"
#include "unordered_dense.h"
#include "tsl/robin_map.h"
#include "tsl/hopscotch_map.h"
// #include "parallel_hashmap/phmap.h"
#include "gtl/phmap.hpp"
#include "sparsepp/spp.h"
// #include "absl/container/flat_hash_map.h"
#include "hash_table8.hpp"

#include "benchmark/benchmark.h"

#include <random>

unsigned int XorShift96() {  /* A George Marsaglia generator, period 2^96-1 */
  static unsigned int x=123456789, y=362436069, z=521288629;
  x ^= x << 16;
  x ^= x >> 5;
  x ^= x << 1;
  unsigned int t = x;
  x = y;
  y = z;
  z = t ^ x ^ y;
  return z;
}

typedef unsigned long long uLL;

std::vector<uLL> keys;
std::vector<uLL> keys_shuffled;

struct custom_hash_simple {
  // using is_avalanching = void;
  auto operator()(unsigned long long const& x) const noexcept -> uint64_t {
    return x;
  }
};

ankerl::unordered_dense::map<unsigned long long, int, custom_hash_simple> ankerl_map;

static void BM_AnkerlMap_ClearAndPush(benchmark::State& state) {
  for (auto _ : state) {
    ankerl_map.clear();
    for (const auto key : keys) {
      ankerl_map[key] = key;
    }
  }
}
BENCHMARK(BM_AnkerlMap_ClearAndPush);

static void BM_AnkerlMap_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys_shuffled) {
      ankerl_map[key] = key;
    }
  }
}
BENCHMARK(BM_AnkerlMap_RandomAccess);

emhash8::HashMap<unsigned long long, int> emhash8_map;

static void BM_Emhash8_ClearAndPush(benchmark::State& state) {
  for (auto _ : state) {
    emhash8_map.clear();
    for (const auto key : keys) {
      emhash8_map[key] = key;
    }
  }
}
BENCHMARK(BM_Emhash8_ClearAndPush);

static void BM_Emhash8_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys_shuffled) {
      emhash8_map[key] = key;
    }
  }
}
BENCHMARK(BM_Emhash8_RandomAccess);

FlatHashMap my_flat_hash_map;

static void BM_FlatHashMap_ClearAndPush(benchmark::State& state) {
  for (auto _ : state) {
    my_flat_hash_map.clear();
    for (const auto key : keys) {
      my_flat_hash_map[key] = key;
    }
  }
}
BENCHMARK(BM_FlatHashMap_ClearAndPush);

static void BM_FlatHashMap_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys_shuffled) {
      my_flat_hash_map[key] = key;
    }
  }
}
BENCHMARK(BM_FlatHashMap_RandomAccess);

static void BM_CalcHash(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys) {
      benchmark::DoNotOptimize((key * 0x9ddfea08eb382d69) >> 44);
    }
  }
}
BENCHMARK(BM_CalcHash);

std::vector<std::pair<int, int>> h_and_ne_random;
std::vector<std::pair<int, int>> h_and_ne_cache;
std::vector<int> la;

void Init() {
  h_and_ne_random.resize(1000000);
  h_and_ne_cache.resize(1000000);
  la.resize(1000000);
  for (int i = 0; i < (int)h_and_ne_random.size(); ++i) {
    h_and_ne_random[i].first = XorShift96() % 1000000;
  }
  for (int i = 0; i < (int)h_and_ne_cache.size(); ++i) {
    h_and_ne_cache[i].first = i;
  }
}

static void BM_CacheRehash(benchmark::State& state) {
  for (auto _ : state) {
    la.assign(1000000, -1);
    for (int i = 0; i < 1000000; ++i) {
      h_and_ne_cache[i].second = la[h_and_ne_cache[i].first];
      la[h_and_ne_cache[i].first] = i;
    }
  }
}
BENCHMARK(BM_CacheRehash);

static void BM_RandomRehash(benchmark::State& state) {
  for (auto _ : state) {
    la.assign(1000000, -1);
    for (int i = 0; i < 1000000; ++i) {
      h_and_ne_random[i].second = la[h_and_ne_random[i].first];
      la[h_and_ne_random[i].first] = i;
    }
  }
}
BENCHMARK(BM_RandomRehash);

static void BM_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (int i = 0; i < 1000000; ++i) {
      for (int j = la[i]; j >= 0; j = h_and_ne_random[j].second) {
        benchmark::DoNotOptimize(h_and_ne_random[j].first);
      }
    }
  }
}
BENCHMARK(BM_RandomAccess);

void InitKeys() {
  constexpr int kNumKeys = 1000000;
  uLL key = 1;
  for (int i = 0; i < kNumKeys; ++i) {
    keys.push_back(key *= 5);
  }
  keys_shuffled = keys;
  std::random_shuffle(keys_shuffled.begin(), keys_shuffled.end());
}

void InitKeys_Random() {
  constexpr int kNumKeys = 1000000;
  for (int i = 0; i < kNumKeys; ++i) {
    keys.push_back(uLL(XorShift96()) << 32 | XorShift96());
  }
  keys_shuffled = keys;
  std::random_shuffle(keys_shuffled.begin(), keys_shuffled.end());
}

void InitKeys_mt19937() {
  constexpr int kNumKeys = 1000000;
  std::mt19937 generator;
  for (int i = 0; i < kNumKeys; ++i) {
    keys.push_back(generator());
  }
  keys_shuffled = keys;
  std::random_shuffle(keys_shuffled.begin(), keys_shuffled.end());
}

old_hash_map my_old_hash_map;
int cnt = 0;

static void BM_MyOldHashMap_ClearAndPush(benchmark::State& state) {
  for (auto _ : state) {
    // AutoTimer my_timer("my_timer");
    cnt++;
    my_old_hash_map.clear();
    for (const auto key : keys) {
      my_old_hash_map[key] = key;
    }
  }
}
BENCHMARK(BM_MyOldHashMap_ClearAndPush);

/*
static void BM_MyOldHashMap_CacheAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys) {
      my_old_hash_map[key] = key;
    }
  }
}
BENCHMARK(BM_MyOldHashMap_CacheAccess);
*/

static void BM_MyOldHashMap_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys_shuffled) {
      my_old_hash_map[key] = key;
    }
  }
}
BENCHMARK(BM_MyOldHashMap_RandomAccess);

new_hash_map my_new_hash_map;

static void BM_MyNewHashMap_ClearAndPush(benchmark::State& state) {
  for (auto _ : state) {
    // AutoTimer my_timer("my_timer");
    cnt++;
    my_new_hash_map.clear();
    for (const auto key : keys) {
      my_new_hash_map[key] = key;
    }
  }
}
BENCHMARK(BM_MyNewHashMap_ClearAndPush);

static void BM_MyNewHashMap_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys_shuffled) {
      my_new_hash_map[key] = key;
    }
  }
}
BENCHMARK(BM_MyNewHashMap_RandomAccess);

VectorHashMap vector_hash_map;

static void BM_VectorHashMap_ClearAndPush(benchmark::State& state) {
  for (auto _ : state) {
    // AutoTimer my_timer("my_timer");
    cnt++;
    vector_hash_map.clear();
    for (const auto key : keys) {
      vector_hash_map[key] = key;
    }
  }
}
BENCHMARK(BM_VectorHashMap_ClearAndPush);

static void BM_VectorHashMap_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys_shuffled) {
      vector_hash_map[key] = key;
    }
  }
}
BENCHMARK(BM_VectorHashMap_RandomAccess);

HybridHashMap hybrid_hash_map;

static void BM_HybridHashMap_ClearAndPush(benchmark::State& state) {
  for (auto _ : state) {
    // AutoTimer my_timer("my_timer");
    cnt++;
    hybrid_hash_map.clear();
    for (const auto key : keys) {
      hybrid_hash_map[key] = key;
    }
  }
}
BENCHMARK(BM_HybridHashMap_ClearAndPush);

static void BM_HybridHashMap_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys_shuffled) {
      hybrid_hash_map[key] = key;
    }
  }
}
BENCHMARK(BM_HybridHashMap_RandomAccess);

// absl::flat_hash_map<unsigned long long, int> google_map;
// static void BM_GoogleMap_ClearAndPush(benchmark::State& state) {
//   for (auto _ : state) {
//     google_map.clear();
//     for (const auto key : keys) {
//       google_map[key] = key;
//     }
//   }
// }
// BENCHMARK(BM_GoogleMap_ClearAndPush);
// static void BM_GoogleMap_RandomAccess(benchmark::State& state) {
//   for (auto _ : state) {
//     for (const auto key : keys_shuffled) {
//       google_map[key] = key;
//     }
//   }
// }
// BENCHMARK(BM_GoogleMap_RandomAccess);

/*
std::unordered_map<unsigned long long, int> map;

static void BM_StlMap_ClearAndPush(benchmark::State& state) {
  for (auto _ : state) {
    map.clear();
    for (const auto key : keys) {
      map[key] = key;
    }
  }
}
BENCHMARK(BM_StlMap_ClearAndPush);

static void BM_StlMap_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys_shuffled) {
      map[key] = key;
    }
  }
}
BENCHMARK(BM_StlMap_RandomAccess);

tsl::robin_map<unsigned long long, int> tsl_map;

static void BM_TslMap_ClearAndPush(benchmark::State& state) {
  for (auto _ : state) {
    tsl_map.clear();
    for (const auto key : keys) {
      tsl_map[key] = key;
    }
  }
}
BENCHMARK(BM_TslMap_ClearAndPush);

static void BM_TslMap_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys_shuffled) {
      tsl_map[key] = key;
    }
  }
}
BENCHMARK(BM_TslMap_RandomAccess);
*/

/*
phmap::flat_hash_map<unsigned long long, int> ph_map;

static void BM_PhMap_ClearAndPush(benchmark::State& state) {
  for (auto _ : state) {
    ph_map.clear();
    for (const auto key : keys) {
      ph_map[key] = key;
    }
  }
}
BENCHMARK(BM_PhMap_ClearAndPush);

static void BM_PhMap_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys_shuffled) {
      ph_map[key] = key;
    }
  }
}
BENCHMARK(BM_PhMap_RandomAccess);
*/

/*
gtl::flat_hash_map<unsigned long long, int> gtl_map;

static void BM_GtlMap_ClearAndPush(benchmark::State& state) {
  for (auto _ : state) {
    gtl_map.clear();
    for (const auto key : keys) {
      gtl_map[key] = key;
    }
  }
}
BENCHMARK(BM_GtlMap_ClearAndPush);

static void BM_GtlMap_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys_shuffled) {
      gtl_map[key] = key;
    }
  }
}
BENCHMARK(BM_GtlMap_RandomAccess);

spp::sparse_hash_map<unsigned long long, int> spp_map;

static void BM_SppMap_ClearAndPush(benchmark::State& state) {
  for (auto _ : state) {
    spp_map.clear();
    for (const auto key : keys) {
      spp_map[key] = key;
    }
  }
}
BENCHMARK(BM_SppMap_ClearAndPush);

static void BM_SppMap_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys_shuffled) {
      spp_map[key] = key;
    }
  }
}
BENCHMARK(BM_SppMap_RandomAccess);
*/

int main(int argc, char** argv) {
  Init();
  InitKeys_Random();
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  printf("cnt = %d\n", cnt);
  return 0;
}
