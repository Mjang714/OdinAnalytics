/**
 * @file axl_demo.cpp
 * @author Derek Huang
 * @brief C++ source for Accel and OXL API integration add-in demo
 * @copyright MIT License
 *
 * This add-in example demonstrates the seamless integration of OXL API
 * functions with the Accel add-in framework. Each function is a duplicate of
 * the OXL exported functions except with the "Ax" instead of "oxl" prefix.
 *
 * @note OXL functions incorrectly take a const reference to a `xloper12*`,
 *  i.e. a `LPXLOPER12`, instead of directly taking the pointer.
 */

#include <concepts>
#include <string>
#include <vector>

#include "oa/accel/addin.h"
#include "oa/accel/matrix_view.h"
#include "oa/accel/oper12.h"
#include "oa/accel/oper12_view.h"
#include "oa/accel/udf.h"
#include "oa/version.h"
#include "oxl/excel_base_funcs.h"
#include "oxl/xl_api/cache_xl_obj.h"
#include "oxl/xl_api/xl_array.h"

namespace oa {

// basic add-in
OA_ACCEL_ADDIN_INSTANCE()
  .name("AXL Demo " + std::string{ODIN_VERSION});

namespace {

/**
 * Visitor to convert any compatible `std::variant<Ts...>` to an `oper12`.
 */
struct var2oper12 {
  /**
   * Convert the variant value to an `oper12` if possible.
   *
   * @tparam T Type convertible to `oper12`
   *
   * @param v Value to convert to `oper12`
   */
  template <typename T>
  requires (std::constructible_from<accel::oper12, T>)
  auto operator()(T v) const
  {
    // note: no implicit conversion unless we list-init from string_view
    return accel::oper12{v};
  }
};

/**
 * Helper function to convert an OXL `XlArray` to an `oper12`.
 *
 * This enables cooperation with the Accel memory management framework.
 *
 * @param arr OXL array to convert
 */
auto to_oper12(const oxl::xl_api::XlArray arr)
{
  // allocate values
  std::vector<accel::oper12> vals(arr.rows() * arr.cols());
  // iterate
  for (auto i = 0u; i < arr.rows(); i++)
    for (auto j = 0u; j < arr.cols(); j++)
      vals[i * arr.cols() + j] = std::visit(var2oper12{}, arr(i, j));
  // create view and return new oper12
  return accel::oper12{{vals.data(), arr.rows(), arr.cols()}};
}

}  // namespace

/**
 * Display the contents of the OXL object given the handle string.
 */
OA_XLL_EXPORT() AxDisplay(/*const*/ xloper12* str) OA_ACCEL_SAFE()
{
  OA_ACCEL_SAFE_RETURN(to_oper12(oxl::OxlDisplay(str)));
}

OA_ACCEL_EXPORT_FUNC(AxDisplay)
  .category("AXL Tools")
  .help("Displays OXL array/dictionary data given the object's string handle")
  .arg("str", "OXL cache string handle");

/**
 * Create an OXL array using the `OxlCreateArray()` function.
 */
OA_XLL_EXPORT() AxArray(/*const*/ xloper12* values) OA_ACCEL_SAFE()
{
  auto obj = oxl::OxlCreateArray(values);
  // TODO: XlCacheObj ctor could automatically generate the base name itself
  auto name = oxl::xl_api::XlCacheObj::GenHandleStr(obj);
  // insert into cache and return string handle
  oxl::xl_api::XlCacheObj cache_obj{obj, name};
  OA_ACCEL_SAFE_RETURN(cache_obj.CacheName());
}

OA_ACCEL_EXPORT_FUNC(AxArray)
  .category("AXL tools")
  .help(
    "Creates an array from the given range data.\n"
    "\n"
    "A string handle to use with AxDisplay() is returned."
  )
  .arg("vals", "2D range to use as array values");

/**
 * Create an OXL dictionary using the `OxlCreateDict()` function.
 */
OA_XLL_EXPORT() AxDictionary(
  /*const*/ xloper12* keys,
  /*const*/ xloper12* values) OA_ACCEL_SAFE()
{
  auto obj = oxl::OxlCreateDict(keys, values);
  // TODO: XlCacheObj ctor could automatically generate the base name itself
  auto name = oxl::xl_api::XlCacheObj::GenHandleStr(obj);
  // insert into cache and return string handle
  oxl::xl_api::XlCacheObj cache_obj{obj, name};
  OA_ACCEL_SAFE_RETURN(cache_obj.CacheName());
}

OA_ACCEL_EXPORT_FUNC(AxDictionary)
  .category("AXL Tools")
  .help(
    "Creates a dictionary from the key and value range data.\n"
    "\n"
    "A string handle to use with AxDisplay() is returned."
  )
  .arg("keys", "1D range to use as dictionary keys")
  .arg("vals", "1D range to use as dictionary values");

}  // namespace oa
