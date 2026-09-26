/**
 * @file ols_demo.cpp
 * @author Derek Huang
 * @brief C++ OLS demo program for linear algebra library testing
 * @copyright MIT License
 */

#include <algorithm>
#include <cmath>
#include <concepts>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iomanip>
#include <iostream>
#include <limits>
#include <random>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <type_traits>
#include <vector>

#include "oa/features.h"  // OA_HAS_OPENBLAS, OA_HAS_EIGEN3, OA_HAS_ARMADILLO
#include "oa/warnings.h"

// either Eigen or OpenBLAS is required
#if !OA_HAS_OPENBLAS && !OA_HAS_EIGEN3
#error "ols_demo.cpp: either OpenBLAS or Eigen3 are required for compilation"
#endif  // !OA_HAS_OPENBLAS && !OA_HAS_EIGEN3

#if OA_HAS_ARMADILLO
#include <armadillo>
#endif  // OA_HAS_ARMADILLO
#if OA_HAS_EIGEN3
#include <Eigen/Core>
#include <Eigen/QR>
#include <Eigen/SVD>
#endif  // OA_HAS_EIGEN3
#if OA_HAS_OPENBLAS
#include <lapacke.h>
#include <openblas_config.h>
#endif  // OA_HAS_OPENBLAS

namespace {

////////////////////////////////////////////////////////////////////////////////

/**
 * OLS solver implementation to use.
 *
 * The options that are available depend on compile/link settings.
 */
enum class ols_backend {
  lapacke,
  eigen3,
  armadillo
};

/**
 * Convert the `ols_backend` value to a string.
 *
 * @param v Enum value
 */
std::string to_string(ols_backend v)
{
  switch (v) {
  case ols_backend::lapacke:
    return "LAPACKE";
  case ols_backend::eigen3:
    return "Eigen3";
  case ols_backend::armadillo:
    return "Armadillo";
  default:
    return "(unknown)";
  }
}

/**
 * OLS solution method to use.
 *
 * This selects the type of matrix factorization to use during solving.
 */
enum class ols_method {
  qr,
  svd
};

/**
 * Convert the `ols_method` value to a string.
 *
 * @param v Enum value
 */
std::string to_string(ols_method v)
{
  switch (v) {
  case ols_method::qr:
    return "QR";
  case ols_method::svd:
    return "SVD";
  default:
    return "(unknown)";
  }
}

/**
 * Floating type to use for calculations.
 */
enum class float_type {
  f32,  // IEEE single precision
  f64   // IEEE double precision
};

/**
 * Convert the `float_type` value to a string.
 *
 * @param v Enum value
 */
std::string to_string(float_type v)
{
  switch (v) {
  case float_type::f32:
    return "f32";
  case float_type::f64:
    return "f64";
  default:
    return "(unknown)";
  }
}

/**
 * Macro defining an `operator<<` for an enum with a `to_string()` overload.
 *
 * @param type Enum type
 */
#define ENUM_OUTPUT_OPERATOR(type) \
  auto& operator<<(std::ostream& out, type v) \
  { \
    return out << to_string(v); \
  } \
  /* require semicolon */ \
  static_assert(true)

ENUM_OUTPUT_OPERATOR(ols_backend);
ENUM_OUTPUT_OPERATOR(ols_method);
ENUM_OUTPUT_OPERATOR(float_type);

#undef ENUM_OUTPUT_OPERATOR

////////////////////////////////////////////////////////////////////////////////

// argument defaults
constexpr std::size_t def_samples = 800u;
constexpr std::size_t def_dims = 8u;
constexpr std::uint32_t def_seed = 8888u;
constexpr auto def_ffmt = float_type::f64;
constexpr auto def_backend =
#if OA_HAS_EIGEN3
  ols_backend::eigen3;
#else
  ols_backend::lapacke;
#endif  // !OA_HAS_EIGEN3
constexpr auto def_method = ols_method::qr;

// program name + usage
const auto progname = std::filesystem::path{__FILE__}.stem().string();
const auto program_usage = "Usage: " + progname + " [-h] [OPTIONS...]\n"
  "\n"
  "Generates sample data and fits and estimator for an OLS regression problem.\n"
  "\n"
  "Given its inputs, this program first generates independent, identity\n"
  "covariance matrix multivariate normal input samples. Outputs are generated\n"
  "by taking the inner product with a known vector of true parameters and then\n"
  "adding white noise. Finally, the maximum likelihood estimator is fit via\n"
  "ordinary linear least squares. The samples and noise are generated using\n"
  "entropy from the available std::mt19937 implementation. Furthermore, the\n"
  "linear model weights being solved for are drawn from a normal distribution\n"
  "with a variance of 2, i.e. twice that of a standard normal, while the white\n"
  "noise is also normal, but with a variance of 0.0001 instead.\n"
  "\n"
  "The available solving methods depend on which linear algebra libraries the\n"
  "program was linked against. Some libraries, e.g. Armadillo, may also require\n"
  "linking against a BLAS/LAPACK[E] implementation regardless."
  "\n"
  "Options:\n"
  "  -h, --help             Print this usage\n"
  "\n"
  "  -n, --samples SAMPLES\n"
  "                         Number of input samples (default " +
    std::to_string(def_samples) + ")\n"
  "\n"
  "  -d, --dims DIMS        Number of input dimensions (default " +
    std::to_string(def_dims) + ")\n"
  "  -s, --seed SEED        PRNG seed value (default " +
    std::to_string(def_seed) + ")\n"
  "\n"
  "  -f, --float-type (f32|f64)\n"
  "                         Floating point format (default " +
    to_string(float_type::f64) + ")\n"
  "\n"
  "  -b, --backend (" +
#if OA_HAS_OPENBLAS
    to_string(ols_backend::lapacke) +
#endif  // OA_HAS_OPENBLAS
#if OA_HAS_EIGEN3
    "|" + to_string(ols_backend::eigen3) +
#endif  // OA_HAS_EIGEN3
#if OA_HAS_ARMADILLO
    "|" + to_string(ols_backend::armadillo) +
#endif  // OA_HAS_ARMADILLO
    ")\n"
  "                         OLS backend implementation (default " +
#if OA_HAS_EIGEN3
    to_string(ols_backend::eigen3) +
#else
    to_string(ols_backend::lapacke) +
#endif  // !OA_HAS_EIGEN3
    ")\n"
  "\n"
  "  -m, --method (" +
    to_string(ols_method::qr) + "|" +
    to_string(ols_method::svd) + ")\n"
  "                         OLS solution method (default " +
    to_string(ols_method::qr) + ")";

/**
 * Command-line options structure.
 *
 * @param help `true` to print program usage
 * @param samples Number of input samples to generate
 * @param dims Number of input features (dimensions)
 * @param seed Seed value for the PRNG
 * @param ffmt Float type, selects either single- or double-precision
 * @param backend OLS implementation backend
 * @param method OLS solution method
 */
struct cli_options {
  bool help = false;
  std::size_t samples = def_samples;
  std::size_t dims = def_dims;
  std::uint32_t seed = def_seed;
  float_type ffmt = def_ffmt;
  ols_backend backend = def_backend;
  ols_method method = def_method;
};

/**
 * Bounds struct for a type.
 *
 * The base template represents no bounds on the type's range at all.
 *
 * @tparam T type
 */
template <typename T = void>
struct bounds {};

/**
 * Partial specialization for integral types.
 *
 * @tparam T Integral type
 */
template <std::integral T>
struct bounds<T> {
  T lower = (std::numeric_limits<T>::min)();
  T upper = (std::numeric_limits<T>::max)();
};

/**
 * SFINAE helper to indicate that the `bounds<T>` has members.
 *
 * @tparam T type
 */
template <typename T, typename = void, typename = void>
struct is_nontrivial_bound : std::false_type {};

/**
 * Partial specialization for `bounds<T>` with actual members.
 *
 * This checks that the `bounds<T>` has `lower` and `upper` members.
 *
 * @tparam T type
 */
template <typename T>
struct is_nontrivial_bound<
  bounds<T>,
  std::void_t<decltype(std::declval<bounds<T>>().lower)>,
  std::void_t<decltype(std::declval<bounds<T>>().upper)> > : std::true_type {};

/**
 * Argument parsing functor.
 *
 * This converts the contents of a string view into the specified type. The
 * base template simply returns the string view contents as-is.
 *
 * Specializations should have `operator()` throw on any errors.
 *
 * @tparam T type
 */
template <typename T = void>
struct from_string {
  /**
   * Return the string view contents as-is and ignore the bounds.
   *
   * @param s Command-line argument
   */
  constexpr auto operator()(std::string_view s, bounds<T>) const noexcept
  {
    return s;
  }
};

/**
 * Partial specialization for integral types.
 *
 * @tparam T Integral type
 */
template <std::integral T>
struct from_string<T> {
  /**
   * Return the selected integral type subject to the bounds.
   *
   * If the bounds are violated or parsing fails an exception is thrown.
   *
   * @param s Command-line argument
   * @param bds Value bounds
   */
  auto operator()(std::string_view s, bounds<T> bds) const
  {
    // convert widest value
    auto v = [s]
    {
      // note: a bit inefficient to convert into std::string first
      if constexpr (std::is_signed_v<T>)
        return std::stoll(std::string{s});
      else
        return std::stoull(std::string{s});
    }();
    // check bounds
    if (v < bds.lower || v > bds.upper)
      throw std::runtime_error{
        "value " + std::to_string(v) + " outside of [" +
        std::to_string(bds.lower) + ", " + std::to_string(bds.upper) + "]"
      };
    // ok, narrow
    return static_cast<T>(v);
  }
};

/**
 * Specialization for `ols_backend`.
 */
template <>
struct from_string<ols_backend> {
  /**
   * Return the `ols_backend` value from the input string.
   *
   * If the string cannot be converted an exception is thrown.
   *
   * @param s Command-line argument
   */
  auto operator()(std::string_view s, bounds<ols_backend>) const
  {
    // note: using constexpr array instead of braced-init-list since MSVC seems
    // to have an issue with destroying initializer list values too early. this
    // was observed during the implementation of the fixed_string<N>
    constexpr ols_backend values[] = {
#if OA_HAS_ARMADILLO
      ols_backend::armadillo,
#endif  // OA_HAS_ARMADILLO
#if OA_HAS_EIGEN3
      ols_backend::eigen3,
#endif  // OA_HAS_EIGEN3
      ols_backend::lapacke
    };
    for (auto v : values)
      if (s == to_string(v))
        return v;
    throw std::runtime_error{std::string{s} + " is not a valid OLS backend"};
  }
};

/**
 * Specialization for `ols_method`.
 */
template <>
struct from_string<ols_method> {
  /**
   * Return the `ols_method` value from the input string.
   *
   * If the string cannot be converted an exception is thrown.
   *
   * @param s Command-line argument
   */
  auto operator()(std::string_view s, bounds<ols_method>) const
  {
    // note: see from_string<ols_backend> comment
    constexpr ols_method values[] = {ols_method::qr, ols_method::svd};
    for (auto v : values)
      if (s == to_string(v))
        return v;
    throw std::runtime_error{std::string{s} + " is not a valid OLS method"};
  }
};

/**
 * Specialization for `float_type`.
 */
template <>
struct from_string<float_type> {
  /**
   * Return the `float_type` value from the input string.
   *
   * If the string cannot be converted an exception is thrown.
   *
   * @param s Command-line argument
   */
  auto operator()(std::string_view s, bounds<float_type>) const
  {
    // note: see from_string<ols_backend> comment
    constexpr float_type values[] = {float_type::f32, float_type::f64};
    for (auto v : values)
      if (s == to_string(v))
        return v;
    throw std::runtime_error{std::string{s} + " is not a valid float type"};
  }
};

/**
 * Parse a value for a command-line argument.
 *
 * This function expects that the current argument is the name of the option
 * and that there should be another argument available for consumption. The
 * converter used is dependent on the template argument type.
 *
 * If bounds are provided they will be checked.
 *
 * @param i Argument option position
 * @param argc Argument count from `main()`
 * @param argv Argument vector from `main()`
 * @param name Option name, e.g. `"-f, --float-size", to display in messages
 * @param out Output variable
 * @param bds Bounds on acceptable values
 * @returns `true` on success, `false` on error
 */
template <typename T>
bool parse(
  int& i,
  int argc,
  char** argv,
  std::string_view name,
  T& out,
  bounds<T> bds = {})
{
  // no additional arguments
  if (++i >= argc) {
    std::cerr << "Error: " << name << ": missing required value" << std::endl;
    return false;
  }
  // convert argument
  // note: could be more generic but we don't need that for this program
  try {
    out = from_string<T>{}(argv[i], bds);
  }
  catch (const std::exception& exc) {
    std::cerr << "Error: " << name << ": exception: " << exc.what() << std::endl;
    return false;
  }
  // done
  return true;
}

/**
 * Parse incoming command-line arguments.
 *
 * @param opts Command-line options to set
 * @param argc Argument count from `main()`
 * @param argv Argument vector from `main()`
 * @returns `true` on success, `false` on error
 */
bool parse_args(cli_options& opts, int argc, char** argv)
{
  for (int i = 1; i < argc; i++) {
    std::string_view arg{argv[i]};
    // -h, --help
    if (arg == "-h" || arg == "--help") {
      opts.help = true;
      return true;
    }
    // -n, --samples
    else if (arg == "-n" || arg == "--samples") {
      if (!parse(i, argc, argv, "-n, --samples", opts.samples))
        return false;
    }
    // -d, --dims
    else if (arg == "-d" || arg == "--dims") {
      if (!parse(i, argc, argv, "-d, --dims", opts.dims))
        return false;
    }
    // -s, --seed
    else if (arg == "-s" || arg == "--seed") {
      if (!parse(i, argc, argv, "-s, --seed", opts.seed))
        return false;
    }
    // -f, --float-type
    else if (arg == "-f" || arg == "--float-type") {
      if (!parse(i, argc, argv, "-f, --float-type", opts.ffmt))
        return false;
    }
    // -b, --backend
    else if (arg == "-b" || arg == "--backend") {
      if (!parse(i, argc, argv, "-b, --backend", opts.backend))
        return false;
    }
    // -m, --method
    else if (arg == "-m" || arg == "--method") {
      if (!parse(i, argc, argv, "-m, --method", opts.method))
        return false;
    }
    // unknown option
    else {
      std::cerr << "Error: Unknown option " << arg << std::endl;
      return false;
    }
  }
  // problem must be overdetermined
  if (opts.samples <= opts.dims) {
    std::cerr << "Error: -n, --samples must be greater than -d, --dims" <<
      std::endl;
    return false;
  }
  // done
  return true;
}

////////////////////////////////////////////////////////////////////////////////

/**
 * Generate input and output samples for a linear least-squares problem.
 *
 * The input and output samples are returned as `std::vector<T>` and should be
 * interpreted as being in column-major order. This is because most linear
 * algebra libraries in C/C++ expect column-major ordering.
 *
 * @tparam G *UniformRandomBitGenerator*
 * @tparam T Floating-point type
 *
 * @param rng PRNG entropy source
 * @param samples Number of input samples
 * @param sol Expected solution vector
 * @returns `std::pair` of input and output values
 */
template <std::uniform_random_bit_generator G, std::floating_point T>
auto make_ols(G& rng, std::size_t samples, std::span<const T> sol)
{
  // input dimension
  auto dims = sol.size();
  // normal dist, input vector, output vector
  std::normal_distribution<T> dist;
  std::vector<T> xs(samples * dims);
  std::vector<T> ys(samples);
  // fill xs with independent normal draws
  // note: this could be faster if we had SIMD implementation
  for (auto& v : xs)
    v = dist(rng);
  // inner product with sol + noise to produce outputs
  // note: we go in column-major order
  for (std::size_t j = 0u; j < dims; j++)
    for (std::size_t i = 0u; i < samples; i++)
      ys[i] += sol[j] * xs[i + samples * j];
  // add noise
  std::normal_distribution<T> ndist(0, 0.01f);
  for (auto& y : ys)
    y += ndist(rng);
  // return
  return std::pair{std::move(xs), std::move(ys)};
}

#if OA_HAS_ARMADILLO
// TODO: document
template <std::floating_point T>
int ols_armadillo(
  ols_method m,
  std::span<const T> xsv,
  std::span<const T> ysv,
  std::span<const T> wsv)
{
  // create dense objects
  // note: writable memory is required for copy-free borrowing
  arma::Mat<T> xs{xsv.data(), ysv.size(), wsv.size()};
  arma::Col<T> ys{ysv.data(), ysv.size()};
  arma::Col<T> ws{wsv.data(), wsv.size()};
  // compute solution vector
  auto whs = [m, &xs, &ys]
  {
    arma::Col<T> w;
    // switch based on method
    switch (m) {
    case ols_method::svd:
      {
        // perform SVD econ factorization
        arma::Mat<T> us;
        arma::Col<T> ss;
        arma::Mat<T> vs;
        arma::svd_econ(us, ss, vs, xs);
        // solve weghted orthogonal system
        arma::solve(w, arma::diagmat(ss) * vs.t(), us.t() * ys);
      }
    default:
      {
        // perform QR econ decomposition
        arma::Mat<T> qs;
        arma::Mat<T> rs;
        arma::qr_econ(qs, rs, xs);
        // solve triangular system
        arma::solve(w, rs, qs.t() * ys);
      }
    }
    // return solution
    return w;
  }();
  // print comparison of the original and solved vectors
  // note: displaying vector transpose so values are in a row, not column
  // note: Armadillo outputs an extra newline when writing objects to stream
  std::cout <<
    "ws: " << ws.t() <<
    "wh: " << whs.t() <<
    // suppress MSVC C4244 from size_t float interop
OA_MSVC_WARNING_PUSH()
OA_MSVC_WARNING_DISABLE(4244)
    // note: 1x1 matrices need explicit conversion to scalar
    "MSE: " << arma::as_scalar((ws - whs).t() * (ws - whs) / ysv.size()) <<
OA_MSVC_WARNING_POP()
      "\n" << std::flush;
  return EXIT_SUCCESS;
}
#endif  // OA_HAS_ARMADILLO

#if OA_HAS_EIGEN3
// TODO: document
template <std::floating_point T>
int ols_eigen3(
  ols_method m,
  std::span<const T> xsv,
  std::span<const T> ysv,
  std::span<const T> wsv)
{
  // convenience type alias
  using eigen3_matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;
  using eigen3_colvec = Eigen::Matrix<T, Eigen::Dynamic, 1>;
  // create read-only views of xsv, ysv, wsv
  // note: no list-init as MSVC emits C2398 error
  Eigen::Map<const eigen3_matrix> xs(xsv.data(), ysv.size(), wsv.size());
  Eigen::Map<const eigen3_colvec> ys(ysv.data(), ysv.size(), 1);
  Eigen::Map<const eigen3_colvec> ws(wsv.data(), wsv.size(), 1);
  // compute solution vector
  auto whs = [m, &xs, &ys]
  {
    eigen3_colvec w;
    // switch based on method
    // note: must assign and then return due to how expression templates work
    switch (m) {
    case ols_method::svd:
      {
      // SVD options
      constexpr auto svd_opts = Eigen::ComputeThinU | Eigen::ComputeThinV;
// in Eigen 5.0 passing the flags as parameters instead of as template
// parameters is deprecated so we have a version check here
//
// note: the definition of EIGEN_VERSION_AT_LEAST() has changed in Eigen 5.0 as
// Eigen has moved to using semantic versioning. the world version is 3 but the
// major version is 5 compared to Eigen 3.4 where the major version is *4*
//
#if EIGEN_VERSION_AT_LEAST(5, 0, 0)
        // note: template keyword required since xs is type-dependent
        w = xs.template bdcSvd<svd_opts>().solve(ys);
#else
        w = xs.bdcSvd(svd_opts).solve(ys);
#endif  // !EIGEN_VERSION_AT_LEAST(5, 0, 0)
      }
    default:
      w = xs.colPivHouseholderQr().solve(ys);
    }
    // return solution
    return w;
  }();
  // print comparison of the original and solved vectors
  // note: displaying vector transpose so values are in a row, not column
  std::cout <<
    "ws: " << ws.transpose() << "\n" <<
    "wh: " << whs.transpose() << "\n" <<
    "MSE: " << ((ws - whs).transpose() * (ws - whs) / ysv.size()) << "\n" <<
    std::flush;
  return EXIT_SUCCESS;
}
#endif  // OA_HAS_EIGEN3

// TODO: document
template <std::floating_point T>
int ols_main(const cli_options& opts)
{
  // seeded PRNG + true linear weights + view
  std::mt19937 rng{opts.seed};
  std::vector<T> ws(opts.dims);
  std::span<const T> wsv{ws};
  // initialize weights
  std::normal_distribution<T> wdist{0, std::sqrt(2.f)};
  for (auto& w : ws)
    w = wdist(rng);
  // create input matrix + noisy outputs + print summary
  auto [xs, ys] = make_ols(rng, opts.samples, wsv);
  std::cout <<
    "input shape: (" << opts.samples << ", " << opts.dims << ")\n" <<
    "OLS backend: " << opts.backend <<
// extra info about BLAS/LAPACK[E] provider if available
#if OA_HAS_OPENBLAS
      [b = opts.backend] {
        switch (b) {
#if defined(ARMA_USE_BLAS) || defined(ARMA_USE_LAPACK)
        case ols_backend::armadillo:
#endif  // defined(ARMA_USE_BLAS) || defined(ARMA_USE_LAPACK)
#if defined(EIGEN_USE_BLAS) || defined(EIGEN_USE_LAPACKE)
        case ols_backend::eigen3:
#endif  // defined(EIGEN_USE_BLAS) || defined(EIGEN_USE_LAPACKE)
        case ols_backend::lapacke:
          return ", " OPENBLAS_VERSION;
        // no external BLAS/LAPACK[E]
        default:
          return "";
        }
      }() <<
#endif  // OA_HAS_OPENBLAS
        // defined(EIGEN_USE_BLAS) || defined(EIGEN_USE_LAPACKE)
      "\n" <<
    "OLS method: " << opts.method << "\n" <<
    "using " << opts.ffmt << " floating-point representation\n" << std::flush;
  // create views for inputs + outputs
  std::span<const T> xsv{xs};
  std::span<const T> ysv{ys};
  // select backend
  switch (opts.backend) {
#if OA_HAS_EIGEN3
  case ols_backend::eigen3:
    return ols_eigen3(opts.method, xsv, ysv, wsv);
#endif  // OA_HAS_EIGEN3
#if OA_HAS_ARMADILLO
  case ols_backend::armadillo:
    return ols_armadillo(opts.method, xsv, ysv, wsv);
#endif  // OA_HAS_ARMADILLO
  default:
    // TODO: not implemented (LAPACKE)
    break;
  }
  return EXIT_SUCCESS;
}

}  // namespace

int main(int argc, char** argv)
{
  // parse arguments
  cli_options opts;
  if (!parse_args(opts, argc, argv))
    return EXIT_FAILURE;
  // print usage if requested
  if (opts.help) {
    std::cout << program_usage << std::endl;
    return EXIT_SUCCESS;
  }
  // delegate to template main
  if (opts.ffmt == float_type::f32)
    return ols_main<float>(opts);
  else
    return ols_main<double>(opts);
}
