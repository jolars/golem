#pragma once

#include <RcppArmadillo.h>
#include <memory>
#include "utils.h"

class Family {
public:
  virtual
  void
  eval(const arma::mat& x,
       const arma::mat& y,
       const arma::vec& intercept,
       const arma::mat& beta) = 0;

  virtual
  double
  primal() = 0;

  virtual
  double
  dual(const arma::mat&y) = 0;

  // this is not really the true gradient, and needs to multiplied by X'
  virtual
  arma::mat
  gradient(const arma::mat& y) = 0;

  virtual
  double
  link(const double y) = 0;
};

class Gaussian : public Family {
private:
  bool fit_intercept;
  arma::vec residuals;
  arma::mat lin_pred;
  double loss = 0.0;

public:
  Gaussian(const bool fit_intercept) : fit_intercept(fit_intercept) {};

  void
  eval(const arma::mat& x,
       const arma::mat& y,
       const arma::vec& intercept,
       const arma::mat& beta)
  {
    lin_pred = x * beta;

    if (fit_intercept)
      lin_pred += intercept(0);

    residuals = lin_pred - y;
    loss = 0.5*std::pow(arma::norm(residuals), 2);
  }

  double
  primal()
  {
    return loss;
  }

  double
  dual(const arma::mat& y)
  {
    return -loss - arma::dot(residuals, y);
  }

  arma::mat
  gradient(const arma::mat& y)
  {
    return lin_pred - y;
  }

  double
  link(const double y)
  {
    return y;
  }
};

class Binomial : public Family {
private:
  const bool fit_intercept;
  arma::mat lin_pred;
  arma::mat exp_y_lin_pred;

public:
  Binomial(const bool fit_intercept) : fit_intercept(fit_intercept) {};

  void
  eval(const arma::mat& x,
       const arma::mat& y,
       const arma::vec& intercept,
       const arma::mat& beta)
  {
    lin_pred = x * beta;

    if (fit_intercept)
      lin_pred += intercept(0);

    exp_y_lin_pred = arma::exp(y % lin_pred);
  }

  double
  primal()
  {
    return arma::accu(arma::log(1.0 + 1.0/exp_y_lin_pred));
  }

  double
  dual(const arma::mat&y)
  {
    using namespace arma;
    const arma::vec r = 1.0/(1.0 + exp_y_lin_pred);
    return arma::as_scalar((r-1.0).t()*log(1.0-r) - r.t()*log(r));
  }

  arma::mat
  gradient(const arma::mat& y)
  {
    return -y / (exp_y_lin_pred + 1.0);
  }

  double
  link(const double y)
  {
    // TODO(johan): consider letting the user choose this
    double pmin = 1e-9;
    double pmax = 1.0 - pmin;
    double z = clamp(y, pmin, pmax);

    return std::log((y + 1.0)/2 / (1.0 - z));
  }
};

// helper to choose family
inline
std::unique_ptr<Family>
setupFamily(const std::string& family_choice,
            const bool fit_intercept)
{
  if (family_choice == "binomial")
    return std::unique_ptr<Binomial>(new Binomial{fit_intercept});
  else
    return std::unique_ptr<Gaussian>(new Gaussian{fit_intercept});
}

