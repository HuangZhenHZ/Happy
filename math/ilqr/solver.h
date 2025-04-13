#pragma once

#include "Eigen/Cholesky"
#include "Eigen/Core"
#include "Eigen/LU"

#include "problem.h"

template <int XDim, int UDim>
class Solver final {
 public:
  using VectorX = Eigen::Matrix<double, XDim, 1>;
  using VectorU = Eigen::Matrix<double, UDim, 1>;
  using MatrixXX = Eigen::Matrix<double, XDim, XDim>;
  using MatrixUU = Eigen::Matrix<double, UDim, UDim>;
  using MatrixUX = Eigen::Matrix<double, UDim, XDim>;
  using MatrixXU = Eigen::Matrix<double, XDim, UDim>;

  using VecOfVectorX = std::vector<VectorX>;
  using VecOfVectorU = std::vector<VectorU>;
  using VecOfMatrixUX = std::vector<MatrixUX>;

  bool Solve(const Problem<XDim, UDim>& problem,
             VecOfVectorU* u_traj);

  static double ComputeTrajCost(
      const Problem<XDim, UDim>& problem,
      const VecOfVectorX& x_traj,
      const VecOfVectorU& u_traj,
      double acceptable_max_cost = std::numeric_limits<double>::infinity());

 private:
  struct IterationVariables {
    double mu = 1.0;
    double delta_mu = 1.0;
    double alpha = std::numeric_limits<double>::signaling_NaN();
    double cost_reduction = 0.0;
    double cost_reduction_ratio = 0.0;
    double grad_norm = 0.0;
    VecOfVectorU k_traj;
    VecOfMatrixUX K_traj;
    Eigen::Vector2d dV;
    bool hessian_all_pd = true;
    VecOfVectorU k_traj_no_reg;
  };

  int reg_type_ = 0;
  int max_num_iter_ = 100;
  double min_grad_thresh_ = 1e-4;
  double min_cost_reduction_thresh_ = 1e-3;
  double min_cost_reduction_ratio_ = 0.0;
  int max_num_iter_ignore_mu_ = 15;

  struct RegParams {
    RegParams() = default;
    explicit RegParams(double max_mu,
                       double min_mu,
                       double delta_mu_factor,
                       double grad_exit_mu_thresh)
        : max_mu(max_mu),
          min_mu(min_mu),
          delta_mu_factor(delta_mu_factor),
          grad_exit_mu_thresh(grad_exit_mu_thresh) {}
    ~RegParams() = default;

    double max_mu = 1e10;
    double min_mu = 1e-6;
    double delta_mu_factor = 1.6;
    double grad_exit_mu_thresh = 1e-5;
  };
  RegParams reg_params_;

  std::vector<double> alpha_vec_ = {
      1.0000, 0.5012, 0.2512, 0.1259, 0.0631, 0.0316, 0.0158, 0.0079, 0.0040, 0.0020, 0.0010};

  static bool ForwardShooting(const Problem<XDim, UDim>& problem,
                              const IterationVariables& vars,
                              const VecOfVectorX& x_traj,
                              const VecOfVectorU& u_traj,
                              VecOfVectorX* x_traj_new,
                              VecOfVectorU* u_traj_new);

  bool BackwardUpdate(
      const Problem<XDim, UDim>& problem,
      const VecOfVectorX& x_traj,
      const VecOfVectorU& u_traj,
      const std::vector<typename CostFunction<XDim, UDim>::Derivatives>& der_traj,
      const typename FinalCostFunction<XDim>::Derivatives& der_final,
      IterationVariables* vars);

  static void IncreaseReg(const RegParams& mu_params, IterationVariables* vars);
  static void DecreaseReg(const RegParams& mu_params, IterationVariables* vars);

  static double ComputeGradNorm(const VecOfVectorU& k_traj, const VecOfVectorU& u_traj);
};

template <int XDim, int UDim>
bool Solver<XDim, UDim>::Solve(const Problem<XDim, UDim>& problem,
                               VecOfVectorU* u_traj_ptr) {
  VecOfVectorX x_traj;
  x_traj.push_back(problem.x0());
  for (int i = 0; i < problem.num_steps(); ++i) {
    x_traj.push_back(problem.dynamic_model(i).f(x_traj[i], (*u_traj_ptr)[i]));
  }
  IterationVariables vars;
  std::vector<double> cost_history;
  double init_cost = ComputeTrajCost(problem, x_traj, *u_traj_ptr);
  cost_history.push_back(init_cost);

  int iter = 0;
  std::vector<typename CostFunction<XDim, UDim>::Derivatives> der_traj;
  for (; iter < max_num_iter_; ++iter) {
    const int len = problem.num_steps();
    typename FinalCostFunction<XDim>::Derivatives der_final;
    der_traj.clear();
    der_traj.resize(len);
    if (problem.has_final_cost_function()) {
      problem.final_cost_function().ComputeCostWithDerivatives(x_traj[len], &der_final);
    }
    for (int i = len - 1; i >= 0; --i) {
      const VectorX& x = x_traj[i];
      const VectorU& u = (*u_traj_ptr)[i];
      problem.cost_function(i).ComputeCostWithDerivatives(x, u, &der_traj[i]);
    }
    bool is_backpass_done = false;
    while (true) {
      if (BackwardUpdate(problem, x_traj, *u_traj_ptr, der_traj, der_final, &vars)) {
        is_backpass_done = true;
        break;
      }
      IncreaseReg(reg_params_, &vars);
      if (vars.mu > reg_params_.max_mu) {
        break;
      }
    }

    vars.grad_norm = ComputeGradNorm(vars.k_traj_no_reg, *u_traj_ptr);
    bool need_termination = false;
    if (vars.grad_norm < min_grad_thresh_ && vars.hessian_all_pd) {
      if (iter >= max_num_iter_ignore_mu_ || vars.mu < reg_params_.grad_exit_mu_thresh) {
        need_termination = true;
      }
    }
    if (need_termination) {
      DecreaseReg(reg_params_, &vars);
      break;
    }

    bool fwd_pass_done = false;
    VecOfVectorX x_traj_new;
    VecOfVectorU u_traj_new;
    double cost_new = std::numeric_limits<double>::signaling_NaN();
    vars.cost_reduction = std::numeric_limits<double>::signaling_NaN();
    double expected_cost_reduction = std::numeric_limits<double>::signaling_NaN();
    if (is_backpass_done) {
      for (int i = 0; i < static_cast<int>(alpha_vec_.size()); ++i) {
        vars.alpha = alpha_vec_[i];
        if (!ForwardShooting(
            problem, vars, x_traj, *u_traj_ptr, &x_traj_new, &u_traj_new)) {
          continue;
        }
        expected_cost_reduction = -vars.alpha * (vars.dV(0) + vars.alpha * vars.dV(1));
        double acceptable_max_cost = cost_history.back() -
            std::max(expected_cost_reduction, 0.0) * min_cost_reduction_ratio_;
        cost_new = ComputeTrajCost(problem, x_traj_new, u_traj_new, acceptable_max_cost);
        if (!std::isfinite(cost_new)) {
          continue;
        }
        vars.cost_reduction = cost_history.back() - cost_new;
        vars.cost_reduction_ratio = 0.0;
        if (expected_cost_reduction > 0.0) {
          vars.cost_reduction_ratio = vars.cost_reduction / expected_cost_reduction;
        } else if (vars.cost_reduction > 1e-6) {
          vars.cost_reduction_ratio = 1;
        } else if (vars.cost_reduction > -1e-6) {
          vars.cost_reduction_ratio = 0;
        } else {
          vars.cost_reduction_ratio = -1;
        }
        if (vars.cost_reduction_ratio > min_cost_reduction_ratio_) {
          fwd_pass_done = true;
          break;
        }
      }
      if (!fwd_pass_done) {
        vars.alpha = std::numeric_limits<double>::signaling_NaN();
      }
    }

    if (fwd_pass_done) {
      x_traj = std::move(x_traj_new);
      *u_traj_ptr = std::move(u_traj_new);
      cost_history.push_back(cost_new);
      DecreaseReg(reg_params_, &vars);
      if (vars.cost_reduction < min_cost_reduction_thresh_) {
        break;
      }
    } else if (is_backpass_done && -(vars.dV(0) + vars.dV(1)) < min_cost_reduction_thresh_) {
      break;
    } else {
      IncreaseReg(reg_params_, &vars);
      if (vars.mu > reg_params_.max_mu) {
        break;
      }
    }
  }

  return iter < max_num_iter_ && vars.mu < reg_params_.max_mu;
}

template <int XDim, int UDim>
bool Solver<XDim, UDim>::ForwardShooting(const Problem<XDim, UDim>& problem,
                                               const IterationVariables& vars,
                                               const VecOfVectorX& x_traj,
                                               const VecOfVectorU& u_traj,
                                               VecOfVectorX* x_traj_new_ptr,
                                               VecOfVectorU* u_traj_new_ptr) {
  VecOfVectorU& u_traj_new = *u_traj_new_ptr;
  VecOfVectorX& x_traj_new = *x_traj_new_ptr;
  u_traj_new.resize(u_traj.size());
  x_traj_new.resize(u_traj.size() + 1);
  x_traj_new[0] = x_traj[0];
  const int len = problem.num_steps();
  for (int i = 0; i < len; ++i) {
    u_traj_new[i] =
        u_traj[i] + vars.alpha * vars.k_traj[i] + vars.K_traj[i] * (x_traj_new[i] - x_traj[i]);
    if (!u_traj_new[i].allFinite()) {
      return false;
    }
    x_traj_new[i + 1] = problem.dynamic_model(i).f(x_traj_new[i], u_traj_new[i]);
    if (!x_traj_new[i + 1].allFinite()) {
      return false;
    }
  }
  return true;
}

template <int XDim, int UDim>
bool Solver<XDim, UDim>::BackwardUpdate(
    const Problem<XDim, UDim>& problem,
    const VecOfVectorX& x_traj,
    const VecOfVectorU& u_traj,
    const std::vector<typename CostFunction<XDim, UDim>::Derivatives>& der_traj,
    const typename FinalCostFunction<XDim>::Derivatives& der_final,
    IterationVariables* vars) {
  const int len = problem.num_steps();
  MatrixXX Vxx = MatrixXX::Zero();
  VectorX Vx = VectorX::Zero();

  vars->k_traj.resize(len);
  vars->k_traj_no_reg.resize(len);
  vars->K_traj.resize(len);
  vars->dV.setZero();
  vars->hessian_all_pd = true;

  if (problem.has_final_cost_function()) {
    Vx = der_final.df_dx;
    Vxx = der_final.d2f_dx_dx;
  }
  for (int i = len - 1; i >= 0; --i) {
    const VectorX& x = x_traj[i];
    const VectorU& u = u_traj[i];
    const typename CostFunction<XDim, UDim>::Derivatives& der = der_traj[i];
    MatrixXX fx = problem.dynamic_model(i).dfdx(x, u);
    MatrixXU fu = problem.dynamic_model(i).dfdu(x, u);

    VectorX Qx = der.df_dx + fx.transpose() * Vx;
    VectorU Qu = der.df_du + fu.transpose() * Vx;
    MatrixXX Qxx = der.d2f_dx_dx + fx.transpose() * Vxx * fx;

    MatrixUX fuTVxx = fu.transpose() * Vxx;
    MatrixUU Quu = der.d2f_du_du + fuTVxx * fu;
    MatrixUX Qux = der.d2f_du_dx + fuTVxx * fx;

    MatrixXX Vxx_reg = Vxx + static_cast<double>(reg_type_ == 1) * vars->mu * MatrixXX::Identity();
    MatrixUX Qux_reg = der.d2f_du_dx + fu.transpose() * Vxx_reg * fx;
    MatrixUU QuuF = der.d2f_du_du + fu.transpose() * Vxx_reg * fu +
                    static_cast<double>(reg_type_ == 0) * vars->mu * MatrixUU::Identity();

    Eigen::LLT<MatrixUU> llt_of_Quu(Quu);
    if (llt_of_Quu.info() == Eigen::NumericalIssue) {
      vars->hessian_all_pd = false;
    } else {
      vars->k_traj_no_reg[i] = llt_of_Quu.solve(Qu);
    }

    Eigen::LLT<MatrixUU> llt_of_QuuF(QuuF);
    if (llt_of_QuuF.info() == Eigen::NumericalIssue) {
      return false;
    }
    vars->k_traj[i] = -llt_of_QuuF.solve(Qu);
    if (!vars->k_traj[i].allFinite()) {
      return false;
    }
    vars->K_traj[i] = -llt_of_QuuF.solve(Qux_reg);
    if (!vars->K_traj[i].allFinite()) {
      return false;
    }

    vars->dV(0) += vars->k_traj[i].transpose() * Qu;
    vars->dV(1) += (0.5 * vars->k_traj[i].transpose() * Quu * vars->k_traj[i]).value();
    Vx = Qx + vars->K_traj[i].transpose() * Quu * vars->k_traj[i] +
         vars->K_traj[i].transpose() * Qu + Qux.transpose() * vars->k_traj[i];
    Vxx = Qxx + vars->K_traj[i].transpose() * Quu * vars->K_traj[i] +
          vars->K_traj[i].transpose() * Qux + Qux.transpose() * vars->K_traj[i];
    Vxx = 0.5 * (Vxx + Vxx.transpose());
  }
  return true;
}

template <int XDim, int UDim>
void Solver<XDim, UDim>::IncreaseReg(const RegParams& mu_params, IterationVariables* vars) {
  vars->delta_mu = std::max(mu_params.delta_mu_factor, mu_params.delta_mu_factor * vars->delta_mu);
  vars->mu = std::max(mu_params.min_mu, vars->mu * vars->delta_mu);
}

template <int XDim, int UDim>
void Solver<XDim, UDim>::DecreaseReg(const RegParams& mu_params, IterationVariables* vars) {
  vars->delta_mu =
      std::min(1.0 / mu_params.delta_mu_factor, vars->delta_mu / mu_params.delta_mu_factor);
  vars->mu =
      static_cast<double>(vars->mu * vars->delta_mu > mu_params.min_mu) * vars->mu * vars->delta_mu;
}

template <int XDim, int UDim>
double Solver<XDim, UDim>::ComputeGradNorm(const VecOfVectorU& k, const VecOfVectorU& u) {
  double sum = 0;
  for (int i = 0; i < static_cast<int>(k.size()); ++i) {
    sum += (k[i].cwiseAbs().array() / (u[i].cwiseAbs().array() + 1.0)).maxCoeff();
  }
  return sum / k.size();
}

template <int XDim, int UDim>
double Solver<XDim, UDim>::ComputeTrajCost(
    const Problem<XDim, UDim>& problem,
    const VecOfVectorX& x_traj,
    const VecOfVectorU& u_traj,
    double acceptable_max_cost) {
  double total_cost = 0.0;
  const int len = problem.num_steps();
  if (problem.has_final_cost_function()) {
    total_cost += problem.final_cost_function().ComputeCostWithDerivatives(x_traj[len]);
    if (!std::isfinite(total_cost) || total_cost > acceptable_max_cost) {
      return std::numeric_limits<double>::infinity();
    }
  }
  for (int i = len - 1; i >= 0; --i) {
    total_cost += problem.cost_function(i).ComputeCostWithDerivatives(x_traj[i], u_traj[i]);
    if (!std::isfinite(total_cost) || total_cost > acceptable_max_cost) {
      return std::numeric_limits<double>::infinity();
    }
  }
  return total_cost;
}
