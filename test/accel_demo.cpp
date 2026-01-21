/**
 * @file accel_demo.cpp
 * @author Derek Huang
 * @brief C++ source for an Accel XLL framework demo XLL
 * @copyright MIT License
 */

#include <numeric>
#include <string>
#include <vector>

#include "oa/accel/addin.h"
#include "oa/accel/fp12_view.h"
#include "oa/accel/oper12.h"
#include "oa/accel/xl_conv.h"  // TODO: stop using xl_conv.h (too low-level)
#include "oa/config/paths.h"
#include "oa/version.h"

namespace oa {

// basic add-in
OA_ACCEL_ADDIN_INSTANCE()
  .name("Accel Demo " + std::string{ODIN_VERSION});

/**
 * Return the location of the OA static data directory.
 */
OA_XLL_EXPORT(const char*) OaDataDir()
{
  // thread-safe since C++11
  static auto path = oa::config::data_dir().string();
  return path.c_str();
}

OA_ACCEL_EXPORT_FUNC(OaDataDir)
  .category("OA Config")
  .help("Return the path to the OA static data directory");

/**
 * Return the maximum of the two values.
 */
OA_XLL_EXPORT(double) OaMax(double a, double b)
{
  return (a > b) ? a : b;
}

OA_ACCEL_EXPORT_FUNC(OaMax)
  .category("OA Math")
  .help("Return the maximum of the two values")
  .arg("a", "First value")
  .arg("b", "Second value");

/**
 * Convert all the characters in the string to upper case.
 */
OA_XLL_EXPORT(xloper12*) OaToUpper(const char* in) OA_ACCEL_SAFE()
{
  std::string str{in};
  // change to uppercase as appropriate
  for (auto& c : str)
    if (c >= 'a' && c <= 'z')
      c -= ('a' - 'A');
  // provide back to Excel
  OA_ACCEL_SAFE_RETURN(accel::oper12{str}.release());
}

OA_ACCEL_EXPORT_FUNC(OaToUpper)
  .category("OA String")
  .help("Convert lowercase characters in the string to upper case")
  .arg("s", "Input string");

/**
 * Compute the single-precision inner product of the two vectors.
 *
 * @note This flattens 2D matrices into vectors in row-major order.
 */
OA_XLL_EXPORT(xloper12*) OaInner(
  const xloper12* a,
  const xloper12* b) OA_ACCEL_SAFE()
{
  // get vectors (must be vectors)
  auto opts = accel::multi_conv_options{}.vector(true);
  auto v1 = accel::as<std::vector<float>>(*a, opts);
  auto v2 = accel::as<std::vector<float>>(*b, opts);
  // error if size mismatch
  if (v1.size() != v2.size())
    throw std::runtime_error{"v1 and v2 have different sizes"};
  // compute inner product + return
  auto res = std::inner_product(v1.begin(), v1.end(), v2.begin(), 0.f);
  OA_ACCEL_SAFE_RETURN(accel::oper12{res}.release());
}

OA_ACCEL_EXPORT_FUNC(OaInner)
  .category("OA Math")
  .help("Compute the single-precision inner product of two vectors")
  .arg("a", "First row or column vector")
  .arg("b", "Second row or column vector");

/**
 * Compute the single-precision matrix multiplication.
 */
OA_XLL_EXPORT(xloper12*) OaMatMul(const FP12* a, const FP12* b) OA_ACCEL_SAFE()
{
  // get views as FP12 is opaque
  accel::fp12_view m1{*a};
  accel::fp12_view m2{*b};
  // dimensions must match
  if (m1.cols() != m2.rows())
    throw std::runtime_error{
      "m1 columns (" + std::to_string(m1.cols()) +
      ") do not match m2 rows (" + std::to_string(m2.rows()) + ")"
    };
  // ok, allocate buffer for result
  std::vector<float> data(m1.rows() * m2.cols());
  // loop
  for (auto i = 0u; i < m1.rows(); i++) {    // rows
    for (auto j = 0u; j < m2.cols(); j++) {  // cols
      auto xij = 0.f;
      // compute inner product
      for (auto k = 0u; k < m1.cols(); k++)
        xij += static_cast<float>(m1[i][k] * m2[k][j]);
      // assign result
      data[i * m2.cols() + j] = xij;
    }
  }
  // create matrix view with correct dimensions, create oper12, release
  accel::matrix_view res{data.data(), m1.rows(), m2.cols()};
  OA_ACCEL_SAFE_RETURN(accel::oper12{res}.release());
}

OA_ACCEL_EXPORT_FUNC(OaMatMul)
  .category("OA Math")
  .help("Compute the matrix inner product of two square arrays")
  .arg("m1", "First matrix shape (n1, n2)")
  .arg("m2", "Second matrix shape (n2, n3)");

}  // namespace oa
