#include "fmt/core.h"
#include "fmt/compile.h"
#include "fmt/os.h"
#include "fmt/printf.h"
#include <chrono>
#include <fstream>

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

const char* filename = "output.txt";
constexpr int kNum = 100000;

int main() {
  {
    AutoTimer timer("iostream");
    std::ofstream file(filename, std::ios::binary);
    for (int i = 0; i < kNum; ++i) {
      file << "i=" << i << ", ";
    }
  }
  {
    AutoTimer timer("fmt");
    auto file = fmt::output_file(filename);
    for (int i = 0; i < kNum; ++i) {
      file.print("i={:d}, ", i);
    }
  }
  {
    AutoTimer timer("iostream & fmt");
    std::ofstream file(filename, std::ios::binary);
    for (int i = 0; i < kNum; ++i) {
      file << fmt::format("i={:d}, ", i);
    }
  }
  {
    AutoTimer timer("fprintf");
    FILE* file = fopen(filename, "wb");
    for (int i = 0; i < kNum; ++i) {
      fprintf(file, "i=%d, ", i);
    }
    fclose(file);
  }
  {
    AutoTimer timer("fmt::fprintf");
    FILE* file = fopen(filename, "wb");
    for (int i = 0; i < kNum; ++i) {
      fmt::fprintf(file, "i=%d, ", i);
    }
    fclose(file);
  }
  {
    AutoTimer timer("fputs & fmt");
    FILE* file = fopen(filename, "wb");
    for (int i = 0; i < kNum; ++i) {
      fputs(fmt::format("i={:d}, ", i).c_str(), file);
    }
    fclose(file);
  }
}
