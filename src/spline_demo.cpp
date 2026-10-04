/**
 * @file spline_demo.cpp
 * @author Derek Huang
 * @brief C++ program spline demo program for linear algebra library testing
 * @copyright MIT License
 */

#include <algorithm>
#include <concepts>
#include <cstddef>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <initializer_list>
#include <map>
#include <ranges>
#include <span>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <utility>
#include <variant>
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
  "Fits a cubic spline to data pairs and prints its values and derivatives.\n"
  "\n"
  "The fitted spline is a natural cubic spline, i.e. the second derivatives at\n"
  "the left endpoint of the first and and right endpoint of the last cubic\n"
  "polynomials are equal and zero. A dense matrix decomposition is used to solve\n"
  "the tridiagonal linear system to compute the spline second derivatives at\n"
  "each of the knot points. Spline values and derivatives up to 4th order are\n"
  "evaluated and displayed in comparison to the values computed by SciPy's own\n"
  "natural cubic spline implementation (values match).\n"
  "\n"
  "All spline and derivative evaluations are organized in a DataFrame-like table\n"
  "using a simple implementation provided within this program.\n"
  "\n"
  "Options:\n"
  "  -h, --help             Print this usage";

/**
 * Command-line options structure.
 *
 * @param help `true` to print program usage
 */
struct cli_options {
  bool help = false;
};

/**
 * Parse incoming command-line options.
 *
 * @param opts Command-line options to fill
 * @param argc Argument count from `main()`
 * @param argv Argument vector from `main()`
 * @returns `true` on success, `false` on error
 */
bool parse_args(cli_options& opts, int argc, char** argv)
{
  for (int i = 1; i < argc; i++) {
    // argument view
    std::string_view arg{argv[i]};
    // -h, --help
    if (arg == "-h" || arg == "--help") {
      opts.help = true;
      return true;
    }
    // unknown option
    else {
      std::cerr << "Error: Unknown option " << arg << std::endl;
      return false;
    }
  }
  // done
  return true;
}

/**
 * Compute the knot point second derivatives given the knots and their values.
 *
 * The Eigen3 partial-pivoting Householder QR decomposition solver is used to
 * solve the linear system and no extra error-checking is performed.
 *
 * This function implements the scheme described on the Wikiversity page here:
 * https://en.wikiversity.org/wiki/Cubic_Spline_Interpolation
 *
 * @tparam T Floating-point type
 *
 * @param xs Knot points
 * @param ys Knot values
 */
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

/**
 * Invocable class representing a natural cubic spline.
 *
 * This class models a natural cubic spline and maintains ownership of the knot
 * points, knot values, and computed knot second derivatives necessary to
 * evaluate the spline or its derivatives at an interpolated or extrapolated
 * point. Derivatives are implemented via invocable proxy objects.
 *
 * A user-defined deduction guide is provided to eliminate in most cases the
 * need to explicitly specify the template type parameter.
 *
 * @tparam T Floating-point type
 */
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

// simple table type
// TODO: document
class data_frame {
public:
  using value_type = std::variant<std::monostate, float, double, std::string>;
  using key_map = std::map<std::string_view, std::size_t>;
  using key_storage = std::vector<std::string>;

  /**
   * Stream formatting visitor for the table value type.
   */
  class stream_formatter {
  public:
    /**
     * Ctor.
     *
     * @param out Output stream
     */
    stream_formatter(std::ostream& out) noexcept : out_{&out} {}

    /**
     * Return a reference to the output stream.
     */
    auto& out() const noexcept { return *out_; }

    /**
     * Format the contained value.
     *
     * This provides flexibility for additional types we might want to support.
     * We print `std::monostate` as `"NA"` to represent a missing value.
     *
     * @tparam T type
     */
    template <typename T>
    auto& operator()(const T& v) const
    {
      if constexpr (std::is_same_v<T, std::monostate>)
        return out() << "NA";
      else
        return out() << v;
    }

  private:
    std::ostream* out_;
  };

  /**
   * String formatting visitor for the table value type.
   */
  class string_formatter {
  public:
    /**
     * Format the contained value.
     *
     * For consistency with the `stream_formatter` this simply delegates.
     *
     * @tparam T type
     */
    template <typename T>
    auto operator()(const T& v) const
    {
      std::stringstream ss;
      stream_formatter{ss}(v);
      // note: since C++20 copy can be replaced with move
      return std::move(ss).str();
    }
  };

private:
  /**
   * Helper traits to constrain the input data range.
   *
   * @tparam R Forward range
   */
  template <typename R>
  static constexpr bool value_range =
    std::convertible_to<std::ranges::range_value_t<R>, value_type>;

  /**
   * Helper traits to constrain row and column key types.
   *
   * @tparam R Forward range
   */
  template <typename R>
  static constexpr bool key_range =
    std::convertible_to<std::ranges::range_value_t<R>, std::string>;

public:
  /**
   * Default ctor.
   */
  data_frame() = default;

  /**
   * Ctor.
   *
   * This constructs from initializer lists with string literal labels.
   *
   * @note Initializer lists cannot be used to deduce C++ template ctor types.
   *
   * @param row_keys Row keys string literals
   * @param col_keys Col keys string literals
   * @param data Data values in row-major order
   */
  data_frame(
    std::initializer_list<const char*> row_keys,
    std::initializer_list<const char*> col_keys,
    std::initializer_list<value_type> data)
    : data_frame{row_keys, col_keys, data, std::monostate{}}
  {}

  /**
   * Ctor.
   *
   * This constructs from nested initializer lists with string literal labels.
   *
   * @note Initializer lists cannot be used to deduce C++ template ctor types.
   *
   * @param row_keys Row keys string literals
   * @param col_keys Col keys string literals
   * @param data Data value rows
   */
  data_frame(
    std::initializer_list<const char*> row_keys,
    std::initializer_list<const char*> col_keys,
    std::initializer_list<std::initializer_list<value_type>> data)
  {
    // data cannot be empty
    // note: empty() was added retroactively so we still need !size()
    if (!data.size())
      throw std::runtime_error{"empty data is not allowed"};
    // number of rows must match
    if (data.size() != row_keys.size())
      throw std::runtime_error{"mismatch in number of rows"};
    // row sizes must be consistent
    for (const auto& row : data)
      if (row.size() != col_keys.size())
        throw std::runtime_error{
          "ragged data row of size " + std::to_string(row.size()) +
          " != expected size " + std::to_string(col_keys.size())
        };
    // initialize row and column keys + mappings
    init(row_keys, row_map_, row_keys_);
    init(col_keys, col_map_, col_keys_);
    // allocate data buffer
    data_ = decltype(data_)(row_keys_.size() * col_keys_.size());
    auto it = data_.begin();
    // iterate to copy rows
    for (const auto& row : data)
      it = std::ranges::copy(row, it).out;
  }

  // TODO: can add ctors for template ranges + initializer list row/col keys

  /**
   * Ctor.
   *
   * This constructs from appropriate C++ ranges of uniform type.
   *
   * @note The extra `std::monostate` parameter is for overload disambiguation.
   *
   * @todo Row and column key ranges need not be random-access.
   *
   * @tparam R Range type with values convertible to `value_type`
   * @tparam RK Range type with values convertible to `std::string`
   * @tparam CK Range type with values convertible to `std::string`
   *
   * @param row_keys Row keys
   * @param col_keys Col keys
   * @param data Data values in row-major order
   */
  template <
    std::ranges::forward_range RK,
    std::ranges::forward_range CK,
    std::ranges::forward_range R >
  requires (key_range<RK> && key_range<CK> && value_range<R>)
  data_frame(RK&& row_keys, CK&& col_keys, R&& data, std::monostate = {})
  {
    // data must match row and column key sizes
    auto n_rows = std::ranges::size(row_keys);
    auto n_cols = std::ranges::size(col_keys);
    auto n_elem = std::ranges::size(data);
    if (n_elem != (n_rows * n_cols))
      throw std::runtime_error{
        "number of data values " + std::to_string(n_elem) +
        " != number of rows " + std::to_string(n_rows) +
        " * number of columns " + std::to_string(n_cols)
      };
    // insert keys + mappings row rows and columns
    init(row_keys, row_map_, row_keys_);
    init(col_keys, col_map_, col_keys_);
    // copy values to data buffer
    data_ = decltype(data_)(n_elem);
    std::ranges::copy(data, data_.begin());
  }

  /**
   * Return a const reference to the row keys.
   */
  auto& row_keys() const noexcept { return row_keys_; }

  /**
   * Return a const reference to the specified row key.
   *
   * @param i Row index
   */
  auto& row_keys(std::size_t i) const noexcept
  {
    return row_keys_[i];
  }

  /**
   * Return a const reference to the column keys.
   */
  auto& col_keys() const noexcept { return col_keys_; }

  /**
   * Return a const refrence to the specified column key.
   *
   * @param i Column index
   */
  auto& col_keys(std::size_t i) const noexcept
  {
    return col_keys_[i];
  }

  /**
   * Return a reference to the value at the given row and column index.
   *
   * @param i Row index
   * @param j Col index
   */
  auto& operator()(std::size_t i, std::size_t j) noexcept
  {
    return data_[i * col_keys_.size() + j];
  }

  /**
   * Return a const reference to the value at the given row and column index.
   *
   * @param i Row index
   * @param j Col index
   */
  auto& operator()(std::size_t i, std::size_t j) const noexcept
  {
    return data_[i * col_keys_.size() + j];
  }

  /**
   * Return the number of rows and columns as an `std::pair`.
   */
  auto shape() const noexcept
  {
    return std::pair{row_keys_.size(), col_keys_.size()};
  }

  /**
   * Return the number of data elements in the table.
   */
  auto size() const noexcept
  {
    return data_.size();
  }

  /**
   * Return the number of rows in the table.
   */
  auto rows() const noexcept
  {
    return row_keys_.size();
  }

  /**
   * Return the number of columns in the table.
   */
  auto cols() const noexcept
  {
    return col_keys_.size();
  }

private:
  key_map row_map_;               // row keys -> row index
  key_storage row_keys_;          // row index -> row keys
  key_map col_map_;               // col keys -> col index
  key_storage col_keys_;          // col index -> col keys
  std::vector<value_type> data_;  // data buffer

  /**
   * Initialize the row or column keys from a forward range.
   *
   * This resizes `keys` to the size of `in` before assigning values.
   *
   * @tparam RK Range type with values convertible to `std::string`
   *
   * @param in Row or column keys
   * @param map Key -> index map
   * @param keys Key storage
   */
  template <std::ranges::forward_range RK>
  requires (key_range<RK>)
  void init(RK&& in, key_map& map, key_storage& keys)
  {
    keys = key_storage(std::ranges::size(in));
    // iterate
    auto i = 0u;
    for (const auto& key : in) {
      keys[i] = std::string{key};
      map[keys[i]] = i;
      i++;
    }
  }
};

/**
 * Stream the `data_frame` to an output stream.
 *
 * All values are formatted according to their default `operator<<` formatting
 * and values are aligned appropriately to produce a textual table format, e.g.
 *
 * @code
 *      c    d
 * a  1.0  2.0
 * b  3.0  4.0
 * @endcode
 *
 * No trailing newline is appending so `std::endl` can be used as usual.
 *
 * @param out Output stream
 * @param data table to write to stream
 */
auto& operator<<(std::ostream& out, const data_frame& data)
{
  // get the row key column print width
  std::size_t row_col_width = 0u;
  for (auto& key : data.row_keys())
    if (key.size() > row_col_width)
      row_col_width = key.size();
  // number of rows and columns
  auto [n_rows, n_cols] = data.shape();
  // get the col key column print widths
  std::vector<std::size_t> col_widths(n_cols);
  // first compare against column keys
  for (std::size_t i = 0u; i < n_cols; i++)
    if (data.col_keys(i).size() > col_widths[i])
      col_widths[i] = data.col_keys(i).size();
  // string formatter + string values to stream later
  data_frame::string_formatter fmt;
  std::vector strs(n_rows, std::vector<std::string>(n_cols));
  // iterate for data values in each column
  for (std::size_t i = 0u; i < n_rows; i++) {
    for (std::size_t j = 0u; j < n_cols; j++) {
      // compute string representation
      strs[i][j] = std::visit(fmt, data(i, j));
      // update col width as necessary
      if (strs[i][j].size() > col_widths[j])
        col_widths[j] = strs[i][j].size();
    }
  }
  // output padding helper
  auto pad_field = [&out](std::size_t n)
  {
    for (std::size_t i = 0u; i < n; i++)
      out.put(' ');
  };
  // write column headers
  // first: padding for row keys w/ +1 for extra spacing
  pad_field(row_col_width + 1u);
  // print each column key
  for (std::size_t i = 0u; i < n_cols; i++) {
    // column key
    auto& key = data.col_keys(i);
    // compute + write padding + write key
    // note: extra +1 to separate from previous fields
    pad_field(1u + col_widths[i] - key.size());
    out.write(key.data(), key.size());
  }
  // iterate for row keys + data values
  for (std::size_t i = 0u; i < n_rows; i++) {
    // row key
    auto& key = data.row_keys(i);
    // newline + print row key + padding for row keys w/ +1 for extra spacing
    out.put('\n');
    out.write(key.data(), key.size());
    pad_field(1u + row_col_width - key.size());
    // iterate for row values
    for (std::size_t j = 0u; j < n_cols; j++) {
      // string value
      auto& str = strs[i][j];
      // compute + write padding + write value
      // note: extra +1 to separate from previous fields
      pad_field(1u + col_widths[j] - str.size());
      out.write(str.data(), str.size());
    }
  }
  // done
  return out;
}

}  // namespace

int main(int argc, char** argv)
{
  // parse command-line arguments
  cli_options opts;
  if (!parse_args(opts, argc, argv))
    return EXIT_FAILURE;
  // print program usage
  if (opts.help) {
    std::cout << program_usage << std::endl;
    return EXIT_SUCCESS;
  }
  // knot points and values
  std::vector xs{1., 2., 3., 4., 5.};
  std::vector ys{2., 4., 6., 5., 7.};
  // fit natural cubic spline
  natural_spline f{xs, ys};
  // first derivative
  // note: f.d() produces the same result
  auto f1 = f.d<1>();
  // second derivative
  // note: f.d().d() or f1.d() produces the same result
  auto f2 = f.d<2>();
  // third derivative
  // note: f.d().d().d() or f1.d().d() or f2.d() produces the same result
  auto f3 = f.d<3>();
  // fourth derivative
  // note: f.d().d().d().d() or f1.d().d().d() or f2.d().d() or f3.d() work too
  auto f4 = f.d<4>();
  // evaluate + organize values in table
  //
  // note: expected values correspond to those from SciPy's CubicSpline with
  // bc_type="natural" for natural cubic spline. SciPy values:
  //
  // f(x):
  // 1.10044643, 2.89955357, 5.30133929, 5.52008929, 5.61830357, 8.38169643
  //
  // f'(x):
  // 1.93303571, 1.93303571, 2.33482143, -1.52232143, 2.25446429, 2.25446429
  //
  // f''(x):
  // -0.80357143, 0.80357143, -2.41071429, -0.16071429, 3.05357143, -3.05357143
  //
  // f'''(x):
  // 1.60714286, 1.60714286, -8.03571429, 12.53571429, -6.10714286, -6.10714286
  //
  // f''''(x):
  // 0, 0, 0, 0, 0, 0
  //
  data_frame df{
    // row labels
    {"x = 0.5", "x = 1.5", "x = 2.5", "x = 3.5", "x = 4.5", "x = 5.5"},
    // column labels
    {"f(x)", "f'(x)", "f''(x)", "f'''(x)", "f''''(x)"},
    // values
    {
      {f(0.5), f1(0.5), f2(0.5), f3(0.5), f4(0.5)},
      {f(1.5), f1(1.5), f2(1.5), f3(1.5), f4(1.5)},
      {f(2.5), f1(2.5), f2(2.5), f3(2.5), f4(2.5)},
      {f(3.5), f1(3.5), f2(3.5), f3(3.5), f4(3.5)},
      {f(4.5), f1(4.5), f2(4.5), f3(4.5), f4(4.5)},
      {f(5.5), f1(5.5), f2(5.5), f3(5.5), f4(5.5)}
    }
  };
  // write table
  std::cout << df << std::endl;
  return EXIT_SUCCESS;
}
