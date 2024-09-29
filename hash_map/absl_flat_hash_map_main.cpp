#include "absl/container/flat_hash_map.h"

int main() {
  absl::flat_hash_map<unsigned long long, int> map;
  map[0] = 1;
  return 0;
}
