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

#include "oa/features.h"  // OA_HAS_EIGEN3

// enable VS Code to define feature macros
// note: __has_include standard since C++17
#ifdef __INTELLISENSE__
// Eigen3
#if __has_include(<Eigen/Core>)
#undef OA_HAS_EIGEN3
#define OA_HAS_EIGEN3 1
#endif  // __has_include(<Eigen/Core>)
#endif  // __INTELLISENSE__

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
   * Traits helper ensuring the forward range type converts to `T`.
   *
   * @tparam R Forward range
   */
  template <typename R>
  static constexpr bool valid_range =
    std::convertible_to<std::ranges::range_value_t<R>, T>;

  /**
   * Proxy class representing the spline's derivative.
   *
   * The scope of the derivative type is tied to the scope of spline.
   *
   * @tparam I Order of the derivative
   */
  template <int I>
  requires (I > 0)
  class derivative {
  public:
    /**
     * Ctor.
     *
     * @param f Natural spline
     */
    derivative(const natural_spline& f) noexcept : f_{&f} {}

    /**
     * Evaluate the spline's derivative at the given point.
     *
     * @param v Point to evaluate at
     */
    auto operator()(T v) const noexcept
    {
      // higher derivatives are zero
      if constexpr (I > 3)
        return T{};
      // nonzero derivatives
      else {
        // reference to knot points, knot values, knot point second derivatives
        auto& xs = f_->xs_;
        auto& ys = f_->ys_;
        auto& ms = f_->ms_;
        // determine bucket. i will be in {0u, ... xs.size() - 2}
        auto i = 0u;
        while ((i + 2u < xs.size()) && (v > xs[i + 1u]))
          i++;
        // compute xs[i + 1] - xs[i] difference + 2 * difference
        auto dx = xs[i + 1] - xs[i];
        auto d2x = 2 * dx;
        // evaluate xs[i + 1] - v and v - xs[i]
        auto dxu = xs[i + 1] - v;
        auto dxd = v - xs[i];
        // third derivative
        if constexpr (I == 3)
          return (ms[i + 1] - ms[i]) / dx;
        // second derivative
        else if constexpr (I == 2)
          return ms[i] * dxu / dx + ms[i + 1] * dxd / dx;
        // first derivative
        else
          return (
            -ms[i] * dxu * dxu / d2x + ms[i + 1] * dxd * dxd / d2x +
            (ys[i + 1] - ys[i]) / dx - (ms[i + 1] - ms[i]) * dx / 6
          );
      }
    }

    /**
     * Return the proxy object representing a higher-order spline derivative.
     *
     * @tparam I_ Order of the derivative
     */
    template <int I_ = 1>
    requires (I_ > 0)
    auto d() const noexcept
    {
      return derivative<I + I_>{*f_};
    }

  private:
    const natural_spline* f_;
  };

public:
  /**
   * Default ctor.
   *
   * Attempting to invoke `operator()` on a default-constructed instance is UB.
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
  requires (valid_range<R1> && valid_range<R2>)
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

  /**
   * Evaluate the spline at the given point.
   *
   * If the value is outside the interpolation interval the corresponding
   * endpoint cubic polynomial will be evaluated for extrapolation.
   *
   * @param v Point to evaluate at
   */
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

  /**
   * Evaluate the spline at the given points.
   *
   * This calls the unary `operator()` for each input and returns a tuple.
   *
   * @tparam T1 First type
   * @tparam T2 Second type
   * @tparam Ts Subsequent types
   *
   * @param v1 First evaluation point
   * @param v2 Second evaluation point
   * @param vs Subsequent evaluation points
   */
  template <std::convertible_to<T> T1, std::convertible_to<T> T2, typename... Ts>
  requires (std::convertible_to<Ts, T> && ...)
  auto operator()(T1 v1, T2 v2, Ts... vs) const noexcept
  {
    return std::tuple{(*this)(v1), (*this)(v2), (*this)(vs)...};
  }

  /**
   * Return the proxy object representing the spline's derivative.
   *
   * @tparam I Derivative order
   */
  template <int I = 1>
  requires (I > 0)
  auto d() const noexcept
  {
    return derivative<I>{*this};
  }

private:
  std::vector<T> xs_;  // knot points
  std::vector<T> ys_;  // knot values
  std::vector<T> ms_;  // knot point second derivatives
};

/**
 * User-defined deduction guide for the `natural_spline<T>`.
 *
 * This deduces to the wider of the two floating range types.
 *
 * @tparam R1 Range of floating values
 * @tparam R2 Range of floating values
 */
template <std::ranges::forward_range R1, std::ranges::forward_range R2>
natural_spline(R1&&, R2&&) -> natural_spline<
  std::common_type_t<
    std::ranges::range_value_t<R1>,
    std::ranges::range_value_t<R2>
  > >;

}  // namespace

int main()
{
  // TODO: parse command-line arguments
  // knot points and values
  std::vector xs{1., 2., 3., 4., 5.};
  std::vector ys{2., 4., 6., 5., 7.};
  // fit natural cubic spline
  natural_spline f{xs, ys};
  // interpolated + extrapolated points
  // note: same values as SciPy's CubicSpline with bc_type="natural", i.e.
  // 1.10044643, 2.89955357, 5.30133929, 5.52008929, 5.61830357, 8.38169643
  std::cout <<
    "f(0.5) = " << f(0.5) << "\n" <<
    "f(1.5) = " << f(1.5) << "\n" <<
    "f(2.5) = " << f(2.5) << "\n" <<
    "f(3.5) = " << f(3.5) << "\n" <<
    "f(4.5) = " << f(4.5) << "\n" <<
    "f(5.5) = " << f(5.5) << "\n" << std::flush;
  // first derivatives
  // note: f.d() produces the same result
  auto f1 = f.d<1>();
  // note: same values as SciPy's CubicSpline with bc_type="natural", i.e.
  // 1.93303571, 1.93303571, 2.33482143, -1.52232143, 2.25446429, 2.25446429
  std::cout <<
    "f'(0.5) = " << f1(0.5) << "\n" <<
    "f'(1.5) = " << f1(1.5) << "\n" <<
    "f'(2.5) = " << f1(2.5) << "\n" <<
    "f'(3.5) = " << f1(3.5) << "\n" <<
    "f'(4.5) = " << f1(4.5) << "\n" <<
    "f'(5.5) = " << f1(5.5) << "\n" << std::flush;
  // second derivatives
  // note: f.d().d() or f1.d() produces the same result
  auto f2 = f.d<2>();
  // note: same values as SciPy's CubicSpline with bc_type="natural", i.e.
  // -0.80357143, 0.80357143, -2.41071429, -0.16071429, 3.05357143, -3.05357143
  std::cout <<
    "f''(0.5) = " << f2(0.5) << "\n" <<
    "f''(1.5) = " << f2(1.5) << "\n" <<
    "f''(2.5) = " << f2(2.5) << "\n" <<
    "f''(3.5) = " << f2(3.5) << "\n" <<
    "f''(4.5) = " << f2(4.5) << "\n" <<
    "f''(5.5) = " << f2(5.5) << "\n" << std::flush;
  // third derivatives
  // note: f.d().d().d() or f1.d().d() or f2.d() produces the same result
  auto f3 = f.d<3>();
  // note: same values as SciPy's CubicSpline with bc_type="natural", i.e.
  // 1.60714286, 1.60714286, -8.03571429, 12.53571429, -6.10714286, -6.10714286
  std::cout <<
    "f'''(0.5) = " << f3(0.5) << "\n" <<
    "f'''(1.5) = " << f3(1.5) << "\n" <<
    "f'''(2.5) = " << f3(2.5) << "\n" <<
    "f'''(3.5) = " << f3(3.5) << "\n" <<
    "f'''(4.5) = " << f3(4.5) << "\n" <<
    "f'''(5.5) = " << f3(5.5) << "\n" << std::flush;
  // fourth derivatives
  // note: f.d().d().d().d() or f1.d().d().d() or f2.d().d() or f3.d() work too
  auto f4 = f.d<4>();
  std::cout <<
    "f''''(0.5) = " << f4(0.5) << "\n" <<
    "f''''(1.5) = " << f4(1.5) << "\n" <<
    "f''''(2.5) = " << f4(2.5) << "\n" <<
    "f''''(3.5) = " << f4(3.5) << "\n" <<
    "f''''(4.5) = " << f4(4.5) << "\n" <<
    "f''''(5.5) = " << f4(5.5) << "\n" << std::flush;
  return EXIT_SUCCESS;
}
