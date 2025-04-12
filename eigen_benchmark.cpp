#include "benchmark/benchmark.h"
#include "Eigen/Core"

static void BM_EigenArray(benchmark::State& state) {
  Eigen::Array<double, 10, 1> a, b;
  for (int i = 0; i < 10; ++i) {
    a(i) = i;
    b(i) = i * i;
  }
  for (auto _ : state) {
    Eigen::Array<double, 10, 1> c = a + b;
    for (int i = 0; i < 10; ++i) {
      benchmark::DoNotOptimize(c(i));
    }
  }
}
BENCHMARK(BM_EigenArray);

static void BM_EigenArrayXd(benchmark::State& state) {
  Eigen::ArrayXd a(10), b(10);
  for (int i = 0; i < 10; ++i) {
    a(i) = i;
    b(i) = i * i;
  }
  for (auto _ : state) {
    Eigen::ArrayXd c = a + b;
    for (int i = 0; i < 10; ++i) {
      benchmark::DoNotOptimize(c(i));
    }
  }
}
BENCHMARK(BM_EigenArrayXd);

static void BM_EigenArrayXdAuto(benchmark::State& state) {
  Eigen::ArrayXd a(10), b(10);
  for (int i = 0; i < 10; ++i) {
    a(i) = i;
    b(i) = i * i;
  }
  for (auto _ : state) {
    auto c = a + b;
    for (int i = 0; i < 10; ++i) {
      benchmark::DoNotOptimize(c(i));
    }
  }
}
BENCHMARK(BM_EigenArrayXdAuto);

static void BM_Array(benchmark::State& state) {
  std::array<double, 10> a, b;
  for (int i = 0; i < 10; ++i) {
    a[i] = i;
    b[i] = i * i;
  }
  for (auto _ : state) {
    for (int i = 0; i < 10; ++i) {
      benchmark::DoNotOptimize(a[i] + b[i]);
    }
  }
}
BENCHMARK(BM_Array);

static void BM_Vector(benchmark::State& state) {
  std::vector<double> a(10), b(10);
  for (int i = 0; i < 10; ++i) {
    a[i] = i;
    b[i] = i * i;
  }
  for (auto _ : state) {
    for (int i = 0; i < 10; ++i) {
      benchmark::DoNotOptimize(a[i] + b[i]);
    }
  }
}
BENCHMARK(BM_Vector);

static void BM_EigenMatrix(benchmark::State& state) {
  std::vector<Eigen::Vector2d> points;
  for (int i = 0; i < 10; ++i) {
    points.emplace_back(i, i * i);
  }
  for (auto _ : state) {
    for (int i = 0; i < 5; ++i) {
      Eigen::Vector2d vec(i + 1, i + 2);
      Eigen::Matrix<double, 2, 2> mat;
      mat << i + 3, i + 4, i + 5, i + 6;
      for (const auto& point : points) {
        Eigen::Vector2d result = mat * (point - vec);
        benchmark::DoNotOptimize(result(0));
        benchmark::DoNotOptimize(result(1));
      }
    }
  }
}
BENCHMARK(BM_EigenMatrix);

static void BM_ManualMatrix(benchmark::State& state) {
  std::vector<std::pair<double, double>> points;
  for (int i = 0; i < 10; ++i) {
    points.emplace_back(i, i * i);
  }
  for (auto _ : state) {
    for (int i = 0; i < 5; ++i) {
      double cx = i + 1;
      double cy = i + 2;
      double a = i + 3;
      double b = i + 4;
      double c = i + 5;
      double d = i + 6;
      for (const auto& point : points) {
        double x_diff = point.first - cx;
        double y_diff = point.second - cy;
        benchmark::DoNotOptimize(x_diff * a + y_diff * b);
        benchmark::DoNotOptimize(x_diff * c + y_diff * d);
      }
    }
  }
}
BENCHMARK(BM_ManualMatrix);

int main(int argc, char** argv) {
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
