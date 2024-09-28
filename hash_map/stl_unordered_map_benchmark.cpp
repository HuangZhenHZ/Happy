#include "benchmark/benchmark.h"

#include <unordered_map>

static void BM_1(benchmark::State& state) {
  std::unordered_map<std::string, int> m;
  const char key[] = "abcdefghijklmnopqrst";
  for (auto _ : state) {
    benchmark::DoNotOptimize(m.find(key));
  }
}
BENCHMARK(BM_1);

static void BM_2(benchmark::State& state) {
  std::unordered_map<std::string, int> m;
  std::string key = "abcdefghijklmnopqrst";
  for (auto _ : state) {
    benchmark::DoNotOptimize(m.find(key));
  }
}
BENCHMARK(BM_2);

static void BM_3(benchmark::State& state) {
  std::unordered_map<std::string, int> m;
  std::string key = "abcdefghijklmnopqrst";
  std::pair<const std::string, int> p = std::make_pair(key, 123);
  for (auto _ : state) {
    m.insert(p);
  }
}
BENCHMARK(BM_3);

static void BM_4(benchmark::State& state) {
  std::unordered_map<std::string, int> m;
  std::string key = "abcdefghijklmnopqrst";
  const std::pair<const std::string, int> p = std::make_pair(key, 123);
  for (auto _ : state) {
    m.insert(p);
  }
}
BENCHMARK(BM_4);

static void BM_5(benchmark::State& state) {
  std::unordered_map<std::string, int> m;
  const std::string key = "abcdefghijklmnopqrst";
  for (auto _ : state) {
    m.emplace(key, 123);
  }
}
BENCHMARK(BM_5);

int main(int argc, char** argv) {
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
