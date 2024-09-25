#include "simple_hash_map.h"
#include "unordered_dense.h"

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

std::vector<unsigned long long> keys;
std::vector<unsigned long long> keys_shuffled;

void InitKeys() {
  constexpr int kNumKeys = 1000000;
  unsigned long long key = 1;
  for (int i = 0; i < kNumKeys; ++i) {
    keys.push_back(key *= 5);
  }
  keys_shuffled = keys;
  std::random_shuffle(keys_shuffled.begin(), keys_shuffled.end());
}

namespace old_hash_map_benchmark {
old_hash_map my_old_hash_map;
void BM() {
  {
    AutoTimer auto_timer("old_hash_map");
    for (const auto key : keys) {
      my_old_hash_map[key] = key;
    }
  }
  {
    AutoTimer auto_timer("old_hash_map");
    for (const auto key : keys) {
      my_old_hash_map[key] = key;
    }
  }
  {
    AutoTimer auto_timer("old_hash_map");
    for (const auto key : keys_shuffled) {
      my_old_hash_map[key] = key;
    }
  }
}
}  // namespace old_hash_map_benchmark

namespace ankerl_benchmark {
struct custom_hash_simple {
  using is_avalanching = void;
  auto operator()(unsigned long long const& x) const noexcept -> uint64_t {
    return x;
  }
};
ankerl::unordered_dense::map<unsigned long long, int, custom_hash_simple> ankerl_map;
void BM() {
  {
    AutoTimer auto_timer("ankerl hash map");
    for (const auto key : keys) {
      ankerl_map[key] = key;
    }
  }
  {
    AutoTimer auto_timer("ankerl hash map");
    for (const auto key : keys) {
      ankerl_map[key] = key;
    }
  }
  {
    AutoTimer auto_timer("ankerl hash map");
    for (const auto key : keys_shuffled) {
      ankerl_map[key] = key;
    }
  }
}
}  // namespace old_hash_map_benchmark

namespace std_benchmark {
std::unordered_map<unsigned long long, int> map;
void BM() {
  {
    AutoTimer auto_timer("std hash map");
    for (const auto key : keys) {
      map[key] = key;
    }
  }
  {
    AutoTimer auto_timer("std hash map");
    for (const auto key : keys) {
      map[key] = key;
    }
  }
  {
    AutoTimer auto_timer("std hash map");
    for (const auto key : keys_shuffled) {
      map[key] = key;
    }
  }
}
}  // namespace old_hash_map_benchmark

int main() {
  InitKeys();
  old_hash_map_benchmark::BM();
  ankerl_benchmark::BM();
  std_benchmark::BM();
  return 0;
}
