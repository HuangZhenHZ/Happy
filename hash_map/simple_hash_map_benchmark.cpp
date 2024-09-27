#include "simple_hash_map.h"
#include "unordered_dense.h"
#include "robin_map.h"
#include "hopscotch_map.h"

#include "benchmark/benchmark.h"

#include <random>

class AutoTimer {
  const std::chrono::time_point<std::chrono::steady_clock> tp_;
  std::string info_;
public:
  AutoTimer(const std::string& str) : tp_(std::chrono::steady_clock::now()), info_(std::move(str)) {}
  ~AutoTimer() {
    auto tp2 = std::chrono::steady_clock::now();
    printf("%s: time = %lld\n", info_.c_str(), std::chrono::duration_cast<std::chrono::microseconds>(tp2 - tp_).count());
  }
};

typedef unsigned long long uLL;

std::vector<uLL> keys;
std::vector<uLL> keys_shuffled;

void InitKeys() {
  constexpr int kNumKeys = 1000000;
  uLL key = 1;
  for (int i = 0; i < kNumKeys; ++i) {
    keys.push_back(key *= 5);
  }
  keys_shuffled = keys;
  std::random_shuffle(keys_shuffled.begin(), keys_shuffled.end());
}

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

static void BM_MyOldHashMap_CacheAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys) {
      my_old_hash_map[key] = key;
    }
  }
}
BENCHMARK(BM_MyOldHashMap_CacheAccess);

static void BM_MyOldHashMap_RandomAccess(benchmark::State& state) {
  for (auto _ : state) {
    for (const auto key : keys_shuffled) {
      my_old_hash_map[key] = key;
    }
  }
}
BENCHMARK(BM_MyOldHashMap_RandomAccess);

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

int main(int argc, char** argv) {
  InitKeys_Random();
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  printf("cnt = %d\n", cnt);
  return 0;
}
