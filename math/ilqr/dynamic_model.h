#pragma once

#include "Eigen/Core"

template <int XDim, int UDim>
class DynamicModel {
 public:
  using VectorX = Eigen::Matrix<double, XDim, 1>;
  using VectorU = Eigen::Matrix<double, UDim, 1>;
  using MatrixXX = Eigen::Matrix<double, XDim, XDim>;
  using MatrixXU = Eigen::Matrix<double, XDim, UDim>;

  DynamicModel() = default;
  virtual ~DynamicModel() = default;

  DynamicModel(const DynamicModel&) = default;
  DynamicModel(DynamicModel&&) noexcept = default;
  DynamicModel& operator=(const DynamicModel&) = default;
  DynamicModel& operator=(DynamicModel&&) noexcept = default;

  virtual VectorX f(const VectorX& x, const VectorU& u) const = 0;
  virtual MatrixXX dfdx(const VectorX& x, const VectorU& u) const = 0;
  virtual MatrixXU dfdu(const VectorX& x, const VectorU& u) const = 0;
};
