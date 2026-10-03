/**
 * @file spline_demo.cpp
 * @author Derek Huang
 * @brief C++ program spline demo program for linear algebra library testing
 * @copyright MIT License
 */

#include <algorithm>
#include <concepts>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>

#include "oa/features.h"

#if OA_HAS_EIGEN3
#include <Eigen/Core>
#include <Eigen/QR>
#endif  // OA_HAS_EIGEN3

namespace {

// program name + usage
const auto progname = std::filesystem::path{__FILE__}.stem().string();
const auto program_usage = "Usage: " + progname + " [-h]\n"
  "\n"
  "Options:\n"
  "  -h, --help             Print this usage";

// solve spline 2nd derivs
// TODO: document. implementation from the Wikiversity page referenced in SciPy
// CubicSpline docs: https://en.wikiversity.org/wiki/Cubic_Spline_Interpolation
// note: no checking
template <std::floating_point T>
auto natural_spline_d2s(std::span<const T> xs, std::span<const T> ys)
{
  // Eigen3 type aliases
  using eigen3_colvec = Eigen::Matrix<T, Eigen::Dynamic, 1>;
  using eigen3_matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;
  // number of elements
  auto n = xs.size();
  // vector of n - 1 differences for xs
  // note: needed later so compute once and keep results
  std::vector<T> dxs(n - 1);
  for (auto i = 0u; i < n - 1; i++)
    dxs[i] = xs[i + 1] - xs[i];
  // vector of n - 1 differences for ys
  // note: needed later so compute once and keep results
  std::vector<T> dys(n - 1);
  for (auto i = 0u; i < n - 1; i++)
    dys[i] = ys[i + 1] - ys[i];
  // vector of n - 2 differences for xs
  // note: needed later so compute once and keep results
  std::vector<T> dx2s(n - 2);
  for (auto i = 0u; i < n - 2; i++)
    dx2s[i] = xs[i + 2] - xs[i];
  // coefficient vector of divided differences (linear system RHS)
  eigen3_colvec bs(n);
  // bs[0] required 0 by natural spline
  bs(0) = 0;
  // bs[1] through bs[n - 2] are 6 * [i - 1, i, i + 1] divided differences
  for (auto i = 1u; i < n - 1; i++)
    bs(i) = 6 * (dys[i] / dxs[i] - dys[i - 1] / dxs[i - 1]) / dx2s[i - 1];
  // bs[n - 1] required 0 by natural spline
  bs(n - 1) = 0;
  // tridiagonal matrix of coefficients (zeroed)
  // TODO: maybe use a sparse solver to reduce memory consumption
  eigen3_matrix ms = eigen3_matrix::Zero(n, n);
  // fill in diagonal
  ms.diagonal().fill(2);
  // ms(0, 1) required 0 by natural spline
  // natural boundary conditions
  ms(0, 1) = 0;
  // fill in lower + upper diagonals
  for (auto i = 1u; i < n - 1; i++) {
    // note: computed to local first for friendlier memory access
    auto u = dxs[i - 1] / dx2s[i - 1];
    // lower + upper diagonal values
    ms(i, i - 1) = u;
    ms(i, i + 1) = 1 - u;
  }
  // ms(n - 1, n - 2) required 0 by natural spline
  ms(n - 1, n - 2) = 0;
  // TODO: enable logging option for debugging?
#if 0
  std::cout << ms << std::endl;
  std::cout << bs << std::endl;
#endif  // 0
  // solve tridiagonal system
  // note: no auto return due to use of expression templates
  eigen3_colvec ws = ms.colPivHouseholderQr().solve(bs);
  // TODO: enable logging option for debugging?
#if 0
  std::cout << ws << std::endl;
#endif  // 0
  return ws;
}

// TODO: document
template <std::floating_point T>
class natural_spline {
private:
  /**
   * Traits helper ensuring the forward range type matches `T`.
   *
   * @tparam R Forward range
   */
  template <typename R>
  static constexpr bool is_valid_range =
    std::is_same_v<std::ranges::range_value_t<R>, T>;

public:
  /**
   * Default ctor.
   */
  natural_spline() = default;

  /**
   * Ctor.
   *
   * Constructs (fits) the natural cubic spline coefficients from monotone
   * increasing knot points and data points.
   *
   * @param x Knot points
   * @param y Data points
   */
  template <std::ranges::forward_range R1, std::ranges::forward_range R2>
  requires (is_valid_range<R1> && is_valid_range<R2>)
  natural_spline(R1&& x, R2&& y)
  {
    // number of knot points (>=2)
    auto n = std::ranges::size(x);
    if (n < 2u)
      throw std::runtime_error{"at least 2 knot points required"};
    // size check
    if (auto ny = std::ranges::size(y); n != ny)
      throw std::runtime_error{
        "number of knot points " + std::to_string(n) +
        " != number of data points " + std::to_string(ny)
      };
    // knots must be monotone
    if (!std::ranges::is_sorted(x))
      throw std::runtime_error{"knot points must be monotone increasing"};
    // allocate + copy to xs_, ys_
    xs_ = decltype(xs_)(n);
    ys_ = decltype(ys_)(n);
    std::ranges::copy(x, xs_.begin());
    std::ranges::copy(y, ys_.begin());
    // allocate ms_
    ms_ = decltype(ms_)(xs_.size());
    // view of ms_ memory
    // note: no list-init as MSVC emits C2398 error
    using eigen3_colvector = Eigen::Matrix<T, Eigen::Dynamic, 1>;
    Eigen::Map<eigen3_colvector> msv(ms_.data(), ms_.size(), 1);
    // solve for spline second derivatives
    // note: explicit template parameter to enable conversion from range
    msv = natural_spline_d2s<T>(x, y);
  }

  // return ith value
  auto operator()(T v) const noexcept
  {
    // determine bucket. i will be in {0u, ... xs_.size() - 2}
    // note: xs_ are monotone increasing so no need to check xs_[i]. we need to
    // check the endpoint of each interval at i + 1 and cannot increment more
    // then xs_.size() - 2 as otherwise the right endpoint is out of bounds
    auto i = 0u;
    while ((i + 2u < xs_.size()) && (v > xs_[i + 1u]))
      i++;
    // compute xs_[i + 1] - xs_[i] difference + 6 * difference
    auto dx = xs_[i + 1] - xs_[i];
    auto d6x = 6 * dx;
    // evaluate xs_[i + 1] - v and v - xs_[i] + cubes
    auto dxu = xs_[i + 1] - v;
    auto dxd = v - xs_[i];
    auto dxu3 = dxu * dxu * dxu;
    auto dxd3 = dxd * dxd * dxd;
    // evaluate polynomial
    return (
      (ms_[i] * dxu3 / d6x) + (ms_[i + 1u] * dxd3 / d6x) +
      (ys_[i] / dx - ms_[i] * dx / 6) * dxu +
      (ys_[i + 1u] / dx - ms_[i + 1u] * dx / 6) * dxd
    );
  }

private:
  std::vector<T> xs_;  // knot points
  std::vector<T> ys_;  // knot values
  std::vector<T> ms_;  // spline second derivatives
};

// TODO: add CTAD

using natural_spline_f64 = natural_spline<double>;
using natural_spline_f32 = natural_spline<float>;

}  // namespace

int main()
{
  // TODO: parse command-line arguments
  std::vector xs{1., 2., 3., 4., 5.};
  std::vector ys{2., 4., 6., 5., 7.};
  natural_spline_f64 f{xs, ys};
  // note: same values as SciPy's CubicSpline with bc_type="natural"
  std::cout <<
    "f(0.5) = " << f(0.5) << "\n" <<
    "f(1.5) = " << f(1.5) << "\n" <<
    "f(2.5) = " << f(2.5) << "\n" <<
    "f(3.5) = " << f(3.5) << "\n" <<
    "f(4.5) = " << f(4.5) << "\n" <<
    "f(5.5) = " << f(5.5) << "\n" << std::flush;
  return EXIT_SUCCESS;
}
