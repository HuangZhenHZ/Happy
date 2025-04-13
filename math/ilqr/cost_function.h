#pragma once

#include "Eigen/Core"

template <int XDim, int UDim>
class CostFunction {
 public:
  using VectorX = Eigen::Matrix<double, XDim, 1>;
  using VectorU = Eigen::Matrix<double, UDim, 1>;
  using MatrixXX = Eigen::Matrix<double, XDim, XDim>;
  using MatrixUU = Eigen::Matrix<double, UDim, UDim>;
  using MatrixUX = Eigen::Matrix<double, UDim, XDim>;

  struct Derivatives {
    VectorX df_dx = VectorX::Zero();
    VectorU df_du = VectorU::Zero();
    MatrixXX d2f_dx_dx = MatrixXX::Zero();
    MatrixUU d2f_du_du = MatrixUU::Zero();
    MatrixUX d2f_du_dx = MatrixUX::Zero();
  };

  CostFunction() = default;
  virtual ~CostFunction() = default;

  virtual double ComputeCostWithDerivatives(
      const VectorX& x,
      const VectorU& u,
      Derivatives* nullable_derivatives = nullptr) const = 0;
};

template <int XDim>
class FinalCostFunction {
 public:
  using VectorX = Eigen::Matrix<double, XDim, 1>;
  using MatrixXX = Eigen::Matrix<double, XDim, XDim>;

  struct Derivatives {
    VectorX df_dx = VectorX::Zero();
    MatrixXX d2f_dx_dx = MatrixXX::Zero();
  };

  FinalCostFunction() = default;
  virtual ~FinalCostFunction() = default;

  virtual double ComputeCostWithDerivatives(
      const VectorX& x,
      Derivatives* nullable_derivatives = nullptr) const = 0;
};
