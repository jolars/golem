#pragma once

#include <RcppArmadillo.h>

using namespace Rcpp;
using namespace arma;

void rescale(cube& intercepts,
             cube& betas,
             const rowvec& x_center,
             const rowvec& x_scale,
             const rowvec& y_center,
             const rowvec& y_scale,
             const bool fit_intercept)
{
  const uword p = betas.n_rows;
  const uword m = betas.n_cols;
  const uword n_sigma = betas.n_slices;

  cube x_bar_beta_sum(1, m, n_sigma, fill::zeros);

  for (uword k = 0; k < m; ++k) {

    for (uword j = 0; j < p; ++j) {
      betas.tube(j, k) *= y_scale(k)/x_scale(j);
      x_bar_beta_sum.tube(0, k) += x_center(j)*betas.tube(j, k);
    }

    if (fit_intercept)
      intercepts.tube(0, k) = intercepts.tube(0, k)*y_scale(k)
                              + y_center(k)
                              - x_bar_beta_sum.tube(0, k);
  }
}
