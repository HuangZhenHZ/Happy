#pragma once

#include "Eigen/Cholesky"
#include "Eigen/Core"
#include "Eigen/LU"

#include "problem.h"

template <int XDim, int UDim>
class MySolver {
public:
  using VectorX = Eigen::Matrix<double, XDim, 1>;
  using VectorU = Eigen::Matrix<double, UDim, 1>;
  using MatrixXX = Eigen::Matrix<double, XDim, XDim>;
  using MatrixUX = Eigen::Matrix<double, UDim, XDim>;
  using MatrixUU = Eigen::Matrix<double, UDim, UDim>;
  using MatrixXU = Eigen::Matrix<double, XDim, UDim>;
  using DynamicModel = DynamicModel<XDim, UDim>;
  using TransitionCost = CostFunction<XDim, UDim>;
  using StateCost = FinalCostFunction<XDim>;
  using Q = typename TransitionCost::Derivatives;
  using V = typename StateCost::Derivatives;

  static bool ComputeKV(const Q &q, VectorU *ku, MatrixUX *kux, V *v) {
    assert(ku != nullptr);
    assert(kux != nullptr);
    assert(v != nullptr);
    Eigen::LLT<MatrixUU> llt_of_quu(q.d2f_du_du);
    if (llt_of_quu.info() == Eigen::NumericalIssue) {
      return false;
    }
    *ku = -llt_of_quu.solve(q.df_du);
    *kux = -llt_of_quu.solve(q.d2f_du_dx);
    v->df_dx = q.df_dx +
               kux->transpose() * q.d2f_du_du * *ku +
               kux->transpose() * q.df_du +
               q.d2f_du_dx.transpose() * *ku;
    v->d2f_dx_dx = q.d2f_dx_dx +
                   kux->transpose() * q.d2f_du_du * *kux +
                   kux->transpose() * q.d2f_du_dx +
                   q.d2f_du_dx.transpose() * *kux;
    return true;
  }

  static Q ComputeQ(const V &v, const MatrixXX &fx, const MatrixXU &fu) {
    return Q {
      .df_dx = fx.transpose() * v.df_dx,
      .df_du = fu.transpose() * v.df_dx,
      .d2f_dx_dx = fx.transpose() * v.d2f_dx_dx * fx,
      .d2f_du_du = fu.transpose() * v.d2f_dx_dx * fu,
      .d2f_du_dx = fu.transpose() * v.d2f_dx_dx * fx,
    };
  }

  void Solve(const Problem<XDim, UDim>& problem,
             std::vector<VectorU> input) {
    int n = problem.num_steps();
    assert(static_cast<int>(input.size()) == n);
    u_ = std::move(input);
    x_.push_back(problem.x0());
    for (int i = 0; i < n; ++i) {
      x_.push_back(problem.dynamic_model(i).f(x_[i], u_[i]));
    }
    q_.resize(n);
    v_.resize(n + 1);
    ku_.resize(n);
    kux_.resize(n);
    for (int iter = 0; iter < 100; ++iter) {
      problem.final_cost_function().ComputeCostWithDerivatives(x_[n], &v_[n]);
      for (int i = n - 1; i >= 0; --i) {
        MatrixXX fx = problem.dynamic_model(i).dfdx(x_[i], u_[i]);
        MatrixXU fu = problem.dynamic_model(i).dfdu(x_[i], u_[i]);
        q_[i] = ComputeQ(v_[i + 1], fx, fu);
        Q derivatives;
        problem.cost_function(i).ComputeCostWithDerivatives(x_[i], u_[i], &derivatives);
        q_[i].df_dx += derivatives.df_dx;
        q_[i].df_du += derivatives.df_du;
        q_[i].d2f_dx_dx += derivatives.d2f_dx_dx;
        q_[i].d2f_du_dx += derivatives.d2f_du_dx;
        q_[i].d2f_du_du += derivatives.d2f_du_du;

        if (!ComputeKV(q_[i], &ku_[i], &kux_[i], &v_[i])) {
          printf("end at iter = %d\n", iter);
          return;
        }
      }

      std::vector<VectorX> last_x = x_;
      for (int i = 0; i < n; ++i) {
        u_[i] += ku_[i] + kux_[i] * (x_[i] - last_x[i]);
        x_[i + 1] = problem.dynamic_model(i).f(x_[i], u_[i]);
      }
    }
  }

  const std::vector<VectorU>& u() { return u_; }

private:
  std::vector<VectorX> x_;
  std::vector<VectorU> u_;
  std::vector<Q> q_;
  std::vector<V> v_;
  std::vector<VectorU> ku_;
  std::vector<MatrixUX> kux_;
};
