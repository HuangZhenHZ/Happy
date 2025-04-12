#include "benchmark/benchmark.h"
#include "Eigen/Core"
#include "math/vec.h"

constexpr int kArrayLength = 100;
static void BM_EigenArray(benchmark::State& state) {
  Eigen::Array<double, kArrayLength, 1> a, b;
  for (int i = 0; i < kArrayLength; ++i) {
    a(i) = i;
    b(i) = i * i;
  }
  for (auto _ : state) {
    Eigen::Array<double, kArrayLength, 1> c = a + b;
    for (int i = 0; i < kArrayLength; ++i) {
      benchmark::DoNotOptimize(c(i));
    }
  }
}
BENCHMARK(BM_EigenArray);

static void BM_EigenArrayNoLoop(benchmark::State& state) {
  Eigen::Array<double, kArrayLength, 1> a, b;
  for (int i = 0; i < kArrayLength; ++i) {
    a(i) = i;
    b(i) = i * i;
  }
  for (auto _ : state) {
    Eigen::Array<double, kArrayLength, 1> c = a + b;
    benchmark::DoNotOptimize(c.sum());
  }
}
BENCHMARK(BM_EigenArrayNoLoop);

static void BM_EigenArrayXd(benchmark::State& state) {
  Eigen::ArrayXd a(kArrayLength), b(kArrayLength);
  for (int i = 0; i < kArrayLength; ++i) {
    a(i) = i;
    b(i) = i * i;
  }
  for (auto _ : state) {
    Eigen::ArrayXd c = a + b;
    for (int i = 0; i < kArrayLength; ++i) {
      benchmark::DoNotOptimize(c(i));
    }
  }
}
BENCHMARK(BM_EigenArrayXd);

static void BM_EigenArrayXdAuto(benchmark::State& state) {
  Eigen::ArrayXd a(kArrayLength), b(kArrayLength);
  for (int i = 0; i < kArrayLength; ++i) {
    a(i) = i;
    b(i) = i * i;
  }
  for (auto _ : state) {
    auto c = a + b;
    for (int i = 0; i < kArrayLength; ++i) {
      benchmark::DoNotOptimize(c(i));
    }
  }
}
BENCHMARK(BM_EigenArrayXdAuto);

static void BM_Array(benchmark::State& state) {
  std::array<double, kArrayLength> a, b;
  for (int i = 0; i < kArrayLength; ++i) {
    a[i] = i;
    b[i] = i * i;
  }
  for (auto _ : state) {
    for (int i = 0; i < kArrayLength; ++i) {
      benchmark::DoNotOptimize(a[i] + b[i]);
    }
  }
}
BENCHMARK(BM_Array);

static void BM_ArrayCopy(benchmark::State& state) {
  std::array<double, kArrayLength> a, b;
  for (int i = 0; i < kArrayLength; ++i) {
    a[i] = i;
    b[i] = i * i;
  }
  for (auto _ : state) {
    std::array<double, kArrayLength> c;
    for (int i = 0; i < kArrayLength; ++i) {
      c[i] = a[i] + b[i];
      benchmark::DoNotOptimize(c[i]);
    }
  }
}
BENCHMARK(BM_ArrayCopy);

static void BM_Vector(benchmark::State& state) {
  std::vector<double> a(kArrayLength), b(kArrayLength);
  for (int i = 0; i < kArrayLength; ++i) {
    a[i] = i;
    b[i] = i * i;
  }
  for (auto _ : state) {
    for (int i = 0; i < kArrayLength; ++i) {
      benchmark::DoNotOptimize(a[i] + b[i]);
    }
  }
}
BENCHMARK(BM_Vector);

static void BM_VectorCopy(benchmark::State& state) {
  std::vector<double> a(kArrayLength), b(kArrayLength);
  for (int i = 0; i < kArrayLength; ++i) {
    a[i] = i;
    b[i] = i * i;
  }
  for (auto _ : state) {
    std::vector<double> c(kArrayLength);
    for (int i = 0; i < kArrayLength; ++i) {
      c[i] = a[i] + b[i];
      benchmark::DoNotOptimize(c[i]);
    }
  }
}
BENCHMARK(BM_VectorCopy);

constexpr int kNumPoints = 50;
constexpr int kNumCases = 10;
static void BM_EigenMatrix(benchmark::State& state) {
  std::vector<Eigen::Vector2d> points;
  for (int i = 0; i < kNumPoints; ++i) {
    points.emplace_back(i, i * i);
  }
  std::array<Eigen::Vector2d, kNumCases> vecs;
  std::array<Eigen::Matrix<double, 2, 2>, kNumCases> mats;
  for (int i = 0; i < kNumCases; ++i) {
    vecs[i] = Eigen::Vector2d(i + 1, i + 2);
    mats[i] << i + 3, i + 4, i + 5, i + 6;
  }
  for (auto _ : state) {
    for (int i = 0; i < kNumCases; ++i) {
      const Eigen::Vector2d& vec = vecs[i];
      const Eigen::Matrix<double, 2, 2>& mat = mats[i];
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
  for (int i = 0; i < kNumPoints; ++i) {
    points.emplace_back(i, i * i);
  }
  for (auto _ : state) {
    for (int i = 0; i < kNumCases; ++i) {
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

static void BM_MyMatrix(benchmark::State& state) {
  std::vector<Matrix<double, 2, 1>> points(kNumPoints);
  for (int i = 0; i < kNumPoints; ++i) {
    points[i](0, 0) = i;
    points[i](1, 0) = i * i;
  }
  std::array<Matrix<double, 2, 1>, kNumCases> vecs;
  std::array<Matrix<double, 2, 2>, kNumCases> mats;
  for (int i = 0; i < kNumCases; ++i) {
    vecs[i](0, 0) = i + 1;
    vecs[i](1, 0) = i + 2;
    mats[i](0, 0) = i + 3;
    mats[i](0, 1) = i + 4;
    mats[i](1, 0) = i + 5;
    mats[i](1, 1) = i + 6;
  }
  for (auto _ : state) {
    for (int i = 0; i < kNumCases; ++i) {
      const Matrix<double, 2, 1>& vec = vecs[i];
      const Matrix<double, 2, 2>& mat = mats[i];
      for (const auto& point : points) {
        Matrix<double, 2, 1> result = mat * (point - vec);
        benchmark::DoNotOptimize(result(0, 0));
        benchmark::DoNotOptimize(result(1, 0));
      }
    }
  }
}
BENCHMARK(BM_MyMatrix);

constexpr int kNumMatrix = 10;
constexpr int kDim = 4;
static void BM_EigenMatrix4x4(benchmark::State& state) {
  std::array<Eigen::Matrix<double, kDim, kDim>, kNumMatrix> a;
  std::array<Eigen::Matrix<double, kDim, kDim>, kNumMatrix> b;
  for (int k = 0; k < kNumMatrix; ++k) {
    for (int i = 0; i < kDim; ++i) {
      for (int j = 0; j < kDim; ++j) {
        a[k](i, j) = i + j + k + 1;
        b[k](i, j) = i * j * k + 1;
      }
    }
  }
  for (auto _ : state) {
    for (int k = 0; k < kNumMatrix; ++k) {
      Eigen::Matrix<double, kDim, kDim> c = a[k] * b[k];
      benchmark::DoNotOptimize(c);
    }
  }
}
BENCHMARK(BM_EigenMatrix4x4);

static void BM_MyMatrix4x4(benchmark::State& state) {
  std::array<Matrix<double, kDim, kDim>, kNumMatrix> a;
  std::array<Matrix<double, kDim, kDim>, kNumMatrix> b;
  for (int k = 0; k < kNumMatrix; ++k) {
    for (int i = 0; i < kDim; ++i) {
      for (int j = 0; j < kDim; ++j) {
        a[k](i, j) = i + j + k + 1;
        b[k](i, j) = i * j * k + 1;
      }
    }
  }
  for (auto _ : state) {
    for (int k = 0; k < kNumMatrix; ++k) {
      Matrix<double, kDim, kDim> c = a[k] * b[k];
      benchmark::DoNotOptimize(c);
    }
  }
}
BENCHMARK(BM_MyMatrix4x4);

constexpr int kNumVec2d = 100;
static void BM_EigenVector2d(benchmark::State& state) {
  std::vector<Eigen::Vector2d> vecs;
  for (int i = 0; i < kNumVec2d; ++i) {
    vecs.emplace_back(i + 1, i + 2);
  }
  for (auto _ : state) {
    for (int i = 0; i + 1 < kNumVec2d; ++i) {
      Eigen::Vector2d c = vecs[i] + vecs[i + 1];
      benchmark::DoNotOptimize(c);
      double dot_result = vecs[i].dot(vecs[i + 1]);
      benchmark::DoNotOptimize(dot_result);
      double cross_result = vecs[i](0) * vecs[i + 1](1) - vecs[i](1) * vecs[i + 1](0);
      benchmark::DoNotOptimize(cross_result);
    }
  }
}
BENCHMARK(BM_EigenVector2d);

static void BM_MyVec2d(benchmark::State& state) {
  std::vector<Vec2d> vecs;
  for (int i = 0; i < kNumVec2d; ++i) {
    vecs.emplace_back(i + 1, i + 2);
  }
  for (auto _ : state) {
    for (int i = 0; i + 1 < kNumVec2d; ++i) {
      Vec2d c = vecs[i] + vecs[i + 1];
      benchmark::DoNotOptimize(c);
      double dot_result = vecs[i].InnerProd(vecs[i + 1]);
      benchmark::DoNotOptimize(dot_result);
      double cross_result = vecs[i].CrossProd(vecs[i + 1]);
      benchmark::DoNotOptimize(cross_result);
    }
  }
}
BENCHMARK(BM_MyVec2d);

class NewVec2d : public Eigen::Vector2d {
public:
  NewVec2d():Eigen::Vector2d() {}
  NewVec2d(double x, double y):Eigen::Vector2d(x, y) {}

  // This constructor allows you to construct MyVectorType from Eigen expressions
  template<typename OtherDerived>
  NewVec2d(const Eigen::MatrixBase<OtherDerived>& other)
      : Eigen::Vector2d(other) {}

  // This method allows you to assign Eigen expressions to MyVectorType
  template<typename OtherDerived>
  NewVec2d& operator=(const Eigen::MatrixBase<OtherDerived>& other) {
    this->Eigen::Vector2d::operator=(other);
    return *this;
  }

  double InnerProd(const NewVec2d& v) const {
    return dot(v);
  }
  double CrossProd(const NewVec2d& v) const {
    return x() * v.y() - y() * v.x();
  }
};

static_assert(sizeof(NewVec2d) == sizeof(double) * 2);

static void BM_NewVec2d(benchmark::State& state) {
  std::vector<NewVec2d> vecs;
  for (int i = 0; i < kNumVec2d; ++i) {
    vecs.emplace_back(i + 1, i + 2);
  }
  for (auto _ : state) {
    for (int i = 0; i + 1 < kNumVec2d; ++i) {
      NewVec2d c = vecs[i] + vecs[i + 1];
      benchmark::DoNotOptimize(c);
      double dot_result = vecs[i].InnerProd(vecs[i + 1]);
      benchmark::DoNotOptimize(dot_result);
      double cross_result = vecs[i].CrossProd(vecs[i + 1]);
      benchmark::DoNotOptimize(cross_result);
    }
  }
}
BENCHMARK(BM_NewVec2d);

int main(int argc, char** argv) {
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
  benchmark::Shutdown();
  return 0;
}
