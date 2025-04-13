#pragma once

#include <memory>

#include "Eigen/Core"

#include "cost_function.h"
#include "dynamic_model.h"

template <int XDim, int UDim>
class Problem {
 public:
  using VectorX = Eigen::Matrix<double, XDim, 1>;
  using VectorU = Eigen::Matrix<double, UDim, 1>;

  Problem() = default;
  virtual ~Problem() = default;

  void set_x0(const VectorX& x0) { x0_ = x0; }

  void AddTimeStep(std::unique_ptr<const CostFunction<XDim, UDim>> cost_function,
                   std::unique_ptr<const DynamicModel<XDim, UDim>> dynamic_model) {
    cost_functions_.push_back(std::move(cost_function));
    dynamic_models_.push_back(std::move(dynamic_model));
    ++num_steps_;
  }

  void AddFinalCostFunction(
      std::unique_ptr<const FinalCostFunction<XDim>> final_cost_function) {
    final_cost_function_ = std::move(final_cost_function);
  }

  const VectorX& x0() const { return x0_; }
  int num_steps() const { return num_steps_; }
  const CostFunction<XDim, UDim>& cost_function(int i) const {
    return *cost_functions_[i];
  }
  bool has_final_cost_function() const { return static_cast<bool>(final_cost_function_); }
  const FinalCostFunction<XDim>& final_cost_function() const {
    return *final_cost_function_;
  }
  const DynamicModel<XDim, UDim>& dynamic_model(int i) const {
    return *dynamic_models_[i];
  }

 private:
  int num_steps_ = 0;
  VectorX x0_ = VectorX::Zero();
  std::vector<std::unique_ptr<const CostFunction<XDim, UDim>>> cost_functions_;
  std::vector<std::unique_ptr<const DynamicModel<XDim, UDim>>> dynamic_models_;
  std::unique_ptr<const FinalCostFunction<XDim>> final_cost_function_;
};
