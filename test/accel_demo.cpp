/**
 * @file accel_demo.cpp
 * @author Derek Huang
 * @brief C++ source for an Accel XLL framework demo XLL
 * @copyright MIT License
 */

#include <cmath>
#include <limits>
#include <map>
#include <optional>
#include <numeric>
#include <string>
#include <utility>
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
OA_XLL_EXPORT() OaToUpper(const char* in) OA_ACCEL_SAFE()
{
  std::string str{in};
  // change to uppercase as appropriate
  for (auto& c : str)
    if (c >= 'a' && c <= 'z')
      c -= ('a' - 'A');
  // provide back to Excel
  OA_ACCEL_SAFE_RETURN(str);
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
OA_XLL_EXPORT() OaInner(const xloper12* a, const xloper12* b) OA_ACCEL_SAFE()
{
  // get vectors (must be vectors)
  auto opts = accel::multi_conv_options{}.vector(true);
  auto v1 = accel::as<std::vector<float>>(*a, opts);
  auto v2 = accel::as<std::vector<float>>(*b, opts);
  // error if size mismatch
  if (v1.size() != v2.size())
    throw std::runtime_error{"v1 and v2 have different sizes"};
  // compute inner product + return
  OA_ACCEL_SAFE_RETURN(std::inner_product(v1.begin(), v1.end(), v2.begin(), 0.f));
}

OA_ACCEL_EXPORT_FUNC(OaInner)
  .category("OA Math")
  .help("Compute the single-precision inner product of two vectors")
  .arg("a", "First row or column vector")
  .arg("b", "Second row or column vector");

/**
 * Compute the single-precision matrix multiplication.
 */
OA_XLL_EXPORT() OaMatMul(const FP12* a, const FP12* b) OA_ACCEL_SAFE()
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
  // create matrix view with correct dimensions + return
  OA_ACCEL_SAFE_RETURN((accel::matrix_view{data.data(), m1.rows(), m2.cols()}));
}

OA_ACCEL_EXPORT_FUNC(OaMatMul)
  .category("OA Math")
  .help("Compute the matrix inner product of two square arrays")
  .arg("m1", "First matrix shape (n1, n2)")
  .arg("m2", "Second matrix shape (n2, n3)");

/**
 * Return the date on which the given dates + values has the max value.
 *
 * The shape of the input must be `(n, 2)` for `n` rows with the Excel integral
 * date value in the first column and value in the second column.
 */
OA_XLL_EXPORT() OaMaxDate(const xloper12* in) OA_ACCEL_SAFE()
{
  // get view as xloper12 is opaque
  accel::oper12_view view{in};
  // if not an array this is an error
  if (view.type() != accel::xltype::multi)
    throw std::runtime_error{
      std::string{"input has type "} + accel::to_string(view.type()) +
      " instead of type " + accel::to_string(accel::xltype::multi)
    };
  // shape must be correct
  if (view.cols() != 2)
    throw std::runtime_error{
      "input has " + std::to_string(view.cols()) +
      " columns instead of the expected 2"
    };
  // date + value maximums to track
  int max_date = 0;
  auto max_value = std::numeric_limits<double>::lowest();
  // loop through dates and times
  for (auto i = 0u; i < view.rows(); i++) {
    // convert date + value
    auto date = view(i, 0).as<int>();
    auto value = view(i, 1).as<double>(accel::strict);
    // update as necessary
    if (value > max_value) {
      max_date = date;
      max_value = value;
    }
  }
  // return date back to Excel
  OA_ACCEL_SAFE_RETURN(max_date);
}

OA_ACCEL_EXPORT_FUNC(OaMaxDate)
  .category("OA Time")
  .help(
    "Return the date corresponding to the maximum input value.\n"
    "\n"
    "The format of the input should consist of rows of a date + a number."
  )
  .arg("in", "Rows of date + time entries");

namespace {

/**
 * Discretely discounted zero-coupon bond.
 *
 * This is used to demonstrate how `xloper12_converter<T>` can be specialized.
 */
class zcb {
public:
  /**
   * Ctor.
   *
   * If not specified the notional defaults to 1.
   *
   * @param id Bond string ID
   * @param yield Bond yield under annual compounding
   * @param maturity Bond maturity
   * @param notional Bond notional
   */
  zcb(std::string id, double yield, double maturity, double notional = 1.)
    : id_{std::move(id)}, yield_{yield}, maturity_{maturity}, notional_{notional}
  {
    // note: we allow negative yield due to QE
    if (maturity_ < 0)
      throw std::runtime_error{"cannot have negative maturity"};
    if (notional_ < 0)
      throw std::runtime_error{"cannot have negative notional"};
  }

  /**
   * Return the bond ID.
   */
  auto& id() const noexcept { return id_; }

  /**
   * Return the annual bond yield.
   */
  auto yield() const noexcept { return yield_; }

  /**
   * Return the bond notional.
   */
  auto notional() const noexcept { return notional_; }

  /**
   * Return the bond maturity.
   */
  auto maturity() const noexcept { return maturity_; }

  /**
   * Return the PV of the bond.
   */
  auto operator()() const noexcept
  {
    return std::pow(1 + yield_, -maturity_);
  }

private:
  std::string id_;
  double yield_;
  double notional_;
  double maturity_;
};

}  // namespace

namespace accel {

/**
 * Converter specialization for the `zcb`.
 *
 * This requires an `xltypeMulti` with the following structure:
 *
 * @code
 * +----------+-----------------+
 * | ID       | <bond name>     |
 * | Yield    | <bond yield>    |
 * | Notional | <bond notional> |
 * | Maturity | <bond maturity> |
 * +----------+-----------------+
 * @endcode
 *
 * The "Notional" field is optional. If omitted, the default is used.
 */
template <>
struct xloper12_converter<zcb> {
  auto operator()(const xloper12& op) const
  {
    // take view as xloper12 is opaque
    oper12_view view{&op};
    // must be xltypeMulti
    if (view.type() != xltype::multi)
      throw std::runtime_error{
        std::string{"input must have type xltypeMulti instead of "} +
        to_string(view.type())
      };
    // must have at most 4 rows
    if (view.rows() > 4)
      throw std::runtime_error{
        "input has " + std::to_string(view.rows()) +
        " rows instead of the maximum of 4"
      };
    // must have 2 columns
    if (view.cols() != 2)
      throw std::runtime_error{
        "input has " + std::to_string(view.cols()) +
        " columns instead of the required 2"
      };
    // optionals for our required fields
    std::optional<std::string> id;
    std::optional<double> yield;
    std::optional<double> notional;
    std::optional<double> maturity;
    // cycle through values
    for (auto i = 0u; i < view.rows(); i++) {
      // field name
      auto name = view(i, 0).as<std::string>(strict);
      // view of value
      auto value = view(i, 1);
      // ID
      if (name == "ID")
        id = value.as<std::string>(strict);
      // yield
      else if (name == "Yield")
        yield = value.as<double>(strict);
      // notional
      else if (name == "Notional")
        notional = value.as<double>(strict);
      // maturity
      else if (name == "Maturity")
        maturity = value.as<double>(strict);
      // unknown
      else
        throw std::runtime_error{"unknown input field \"" + name + "\""};
    }
    // if optional has not been set, error
    if (!id)
      throw std::runtime_error{"missing required field ID"};
    if (!yield)
      throw std::runtime_error{"missing required field Yield"};
    // note: notional can be defaulted to 1
    if (!notional)
      notional = 1.;
    if (!maturity)
      throw std::runtime_error{"missing required field Maturity"};
    // otherwise, return our new bond
    return zcb{std::move(*id), *yield, *maturity, *notional};
  }
};

}  // namespace accel

/**
 * Return the price for a zero-coupon bond with annual compounding.
 */
OA_XLL_EXPORT() OaZeroBondPV(const xloper12* in) OA_ACCEL_SAFE()
{
  // convert to ZCB + return PV to Excel
  OA_ACCEL_SAFE_RETURN(accel::as<zcb>(*in)());
}

OA_ACCEL_EXPORT_FUNC(OaZeroBondPV)
  .category("OA Bonds")
  // note: cannot exceed 255 chars or Excel will fail to register the function
  .help(
    "Return the price of an annually compounding zero bond.\n"
    "\n"
    "The input array has a key-value form of the following:\n"
    "\n"
    "ID: bond name\n"
    "Yield: annual bond yield\n"
    "Notional: bond notional (optional)\n"
    "Maturity: bond maturity\n"
    "\n"
    "If omitted the notional defaults to 1."
  )
  .arg("in", "Key-value array of ZCB input fields");

/**
 * Return the ID cheaper of the two annually-compounded zero bonds.
 */
OA_XLL_EXPORT() OaCheapestOfZCB(
  const xloper12* a,
  const xloper12* b) OA_ACCEL_SAFE()
{
  // convert to ZCBs
  auto bond_a = accel::as<zcb>(*a);
  auto bond_b = accel::as<zcb>(*b);
  // return ID of cheaper bond to Excel
  OA_ACCEL_SAFE_RETURN((bond_a() < bond_b()) ? bond_a.id() : bond_b.id());
}

OA_ACCEL_EXPORT_FUNC(OaCheapestOfZCB)
  .category("OA Bonds")
  .help(
    "Return the ID of the cheaper of the two annually-compounded zero bonds.\n"
    "\n"
    "Each input array has a key-value form requiring the \"ID\", \"Yield\", "
    "and \"Maturity\" fields, with \"Notional\" being optional (default 1)."
  )
  .arg("a", "Key-value array of ZCB input fields")
  .arg("b", "Key-value array of ZCB input fields");

/**
 * Return a table of the character frequencies in an input string.
 *
 * The format of the table is as follows:
 *
 * @code
 * +------+------+
 * | Char | Freq |
 * +------+------+
 * | a    | ...  |
 * | ...  | ...  |
 * +------+------+
 * @endcode
 *
 * The header of the table consists of the strings "Char" and "Freq" and each
 * subsequent row consists of an ASCII character and its frequency.
 */
OA_XLL_EXPORT() OaCharFreq(const char* s) OA_ACCEL_SAFE()
{
  // map of characters to frequencies
  std::map<char, unsigned> map;
  // iterate to populate map
  while (*s != '\0')
    map[*s++]++;
  // create oper12 to represent table
  std::vector<accel::oper12> tab_data{"Char", "Freq"};
  // populate using map keys + values
  // note: widen since oper12 cannot directly store unsigned as int
  for (auto [k, v] : map) {
    tab_data.emplace_back(std::string{k});
    tab_data.emplace_back(double(v));
  }
  // return array back to Excel after correctly taking view
  accel::matrix_view tab_view{tab_data.data(), tab_data.size() / 2u, 2u};
  OA_ACCEL_SAFE_RETURN(tab_view);
}

OA_ACCEL_EXPORT_FUNC(OaCharFreq)
  .category("OA String")
  .help(
    "Return a table showing the frequencies of characters in the input.\n"
    "\n"
    "This function returns a table with \"Char\" and \"Freq\" as the header."
  )
  .arg("s", "Input string to get character frequency table for");

}  // namespace oa
