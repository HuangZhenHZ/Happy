#include "solver.h"

#include <iostream>

class MyDynamicModel : public DynamicModel<1, 1> {
 public:
  MyDynamicModel() = default;
  ~MyDynamicModel() override = default;

  VectorX f(const VectorX& x, const VectorU& u) const override {
    if (u(0) > 0) {
      return x + u * u;
    } else {
      return x - u * u;
    }
  }

  MatrixXX dfdx(const VectorX&, const VectorU&) const override {
    MatrixXX res;
    res << 1.0;
    return res;
  }

  MatrixXU dfdu(const VectorX&, const VectorU& u) const override {
    MatrixXU res;
    res << 2.0 * std::abs(u(0));
    return res;
  }
};

class NumericDervDynamicModel : public DynamicModel<1, 1> {
 public:
  NumericDervDynamicModel() = default;
  ~NumericDervDynamicModel() override = default;

  VectorX f(const VectorX& x, const VectorU& u) const override {
    if (u(0) > 0) {
      return x + u * u;
    } else {
      return x - u * u;
    }
  }
};

class MyCostFunction : public CostFunction<1, 1> {
 public:
  MyCostFunction() = default;
  ~MyCostFunction() override = default;

  double ComputeCostWithDerivatives(
      const VectorX& /* x */,
      const VectorU& u,
      Derivatives* nullable_derivatives) const override {
    double cost = 100.0 * u(0) * u(0);
    if (nullable_derivatives) {
      *nullable_derivatives = {};
      nullable_derivatives->df_du(0) = 200.0 * u(0);
      nullable_derivatives->d2f_du_du(0) = 200.0;
    }
    return cost;
  }
};

class MyFinalCostFunction : public FinalCostFunction<1> {
 public:
  MyFinalCostFunction() = default;
  ~MyFinalCostFunction() override = default;

  double ComputeCostWithDerivatives(
      const VectorX& x,
      Derivatives* nullable_derivatives) const override {
    double cost = x(0) * x(0);
    if (nullable_derivatives) {
      *nullable_derivatives = {};
      nullable_derivatives->df_dx(0) = 2.0 * x(0);
      nullable_derivatives->d2f_dx_dx(0) = 2.0;
    }
    return cost;
  }
};

int main() {
  Problem<1, 1> problem;
  problem.set_x0(Problem<1, 1>::VectorX(100.0));
  Solver<1, 1>::VecOfVectorU output;
  for (int i = 0; i < 3; ++i) {
    problem.AddTimeStep(std::make_unique<MyCostFunction>(), std::make_unique<MyDynamicModel>());
    output.emplace_back(-2.0);
  }
  problem.AddFinalCostFunction(std::make_unique<MyFinalCostFunction>());
  Solver<1, 1> solver;
  solver.Solve(problem, &output);
  for (const auto& vector_u : output) {
    std::cout << vector_u << std::endl;
  }
  return 0;
}
