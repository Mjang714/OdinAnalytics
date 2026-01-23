/**
 * @file udf.h
 * @author Derek Huang
 * @brief C++ header for XLL UDF export and registration helpers
 * @copyright MIT License
 */

#ifndef OA_ACCEL_UDF_H_
#define OA_ACCEL_UDF_H_

#include <cstddef>
#include <string>
#include <tuple>
#include <utility>
#include <vector>

#include "oa/common.h"  // for OA_CONCAT(), OA_STRINGIFY()

// forward decls to avoid pulling in XLCALL.H
struct _FP12;
struct xloper12;

// note: we can mix typedefs and using decls as long as they are consistent
using FP12 = _FP12;

namespace oa {
namespace accel {

namespace detail {

/**
 * Traits type to extract the input and return types of an exported function.
 *
 * Since the functions must be `__stdcall` extern "C" functions we do not have
 * to have the same level of genericity as we don't worry about functors or any
 * member functions, we add more complexity.
 *
 * @tparam T type
 */
template <typename T>
struct func_traits {};

/**
 * Partial specialization for an exported DLL function.
 *
 * @tparam R Function return type
 * @tparam Ts Function argument types
 */
template <typename R, typename... Ts>
struct func_traits<R(__stdcall *)(Ts...)> {
  using result_type = R;
  using arg_types = std::tuple<Ts...>;
};

/**
 * Traits type mapping a C++ type to the Excel data type string.
 *
 * Each valid specialization has a `constexpr const char[]` static member
 * `value` that provides the type string, e.g. `"Q"`, etc.
 *
 * @tparam T type
 */
template <typename T>
struct xll_type_text {};

/**
 * Macro defining a `xll_type` specialization.
 *
 * @param type C++ type
 * @param text Excel `xlfRegister` type text
 */
#define OA_ACCEL_XLL_TYPE_MAPPING(type, text) \
  template <> \
  struct xll_type_text<type> { \
    static constexpr const char value[] = text; \
  }

// specializations for known mappings. this is not extensive as we cannot map
// multiple arguments into a single type string, e.g. "O%", and cannot map the
// same C/C++ type to two different strings, e.g. for "F%", "G%". we also don't
// support many of the in-place modifications; this makes reasoning about the
// function more difficult and is not possible via just type information.

/**
 * Type mapping for `double`.
 */
OA_ACCEL_XLL_TYPE_MAPPING(double, "B");

/**
 * Type mapping for an input null-terminated string.
 *
 * We add const-qualification to indicate that this is input-only.
 */
OA_ACCEL_XLL_TYPE_MAPPING(const char*, "C");

/**
 * Type mapping for an unsigned short.
 */
OA_ACCEL_XLL_TYPE_MAPPING(unsigned short, "H");

/**
 * Type mapping for a signed short.
 *
 * Since booleans come from Excel as a `short` we always treat the short as a
 * boolean; there is already a 32-bit integer type we can use.
 */
OA_ACCEL_XLL_TYPE_MAPPING(short, "A");

/**
 * Type mapping for a signed integer.
 *
 * This should be used in most cases for integral arguments.
 */
OA_ACCEL_XLL_TYPE_MAPPING(int, "J");

/**
 * Type mapping for the `FP12` floating-point grid.
 */
OA_ACCEL_XLL_TYPE_MAPPING(const FP12*, "K%");

/**
 * Type mapping for a 16-bit character null-terminated input string.
 *
 * We add const-qualification to indicate that this is input-only.
 */
OA_ACCEL_XLL_TYPE_MAPPING(const unsigned short*, "C%");

/**
 * Type mapping for an input `xloper12`.
 *
 * We use `"Q"` to automatically convert references into values.
 */
OA_ACCEL_XLL_TYPE_MAPPING(const xloper12*, "Q");

/**
 * Type mapping for an output `xloper12`.
 *
 * We use `"Q"` to automatically convert references into values.
 */
OA_ACCEL_XLL_TYPE_MAPPING(xloper12*, "Q");

/**
 * `xll_type_text<R(__stdcall *)(Ts...)` implementation.
 *
 * This takes the function return and argument types and constructs the
 * `pxTypeText` prototype by iterating through each `xll_type_text<T>` string.
 *
 * @tparam T Return type
 * @tparam Ts Argument types
 */
template <typename T, typename... Ts>
struct xll_type_text_impl
  : xll_type_text_impl<
      // indices ascending from 0 for type text for T minus null terminator
      std::make_index_sequence<sizeof xll_type_text<T>::value - 1u>,
      std::make_index_sequence<
        (sizeof xll_type_text<Ts>::value + ...) -  // remaining type text
        sizeof...(Ts)                              // null terminators
      >,
      T,
      Ts...
    > {};

/**
 * Partial specialization for a function with no arguments.
 *
 * @tparam T Return type
 */
template <typename T>
struct xll_type_text_impl<T> : xll_type_text<T> {};

/**
 * `xll_type_text<R(__stdcall *)(Ts...)` index sequence implementation.
 *
 * This performs the real work of compile-time array creation by recursively
 * copying characters at compile time into a `constexpr const char[]`. Indexing
 * is accomplished via the `index_sequence<...>` partial specializations, where
 * `Is...` indexes `xll_type_text<T>::value`, excluding the trailing `'\0'`,
 * and `Js...` indexes `xll_type_text<Ts...>::value`, excluding the `'\0'`.
 *
 * @tparam Is Indices ascending from 0 for indexing the type text of `T`
 * @tparam Js Indices ascending from 0 for indexing the type text of `Ts`
 */
template <std::size_t... Is, std::size_t... Js, typename T, typename... Ts>
struct xll_type_text_impl<
  std::index_sequence<Is...>,  // length of xll_type_text<T>::value - 1
  std::index_sequence<Js...>,  // length of remaining type texts
  T,
  Ts...
> {
  static constexpr const char value[] = {
    // copy type text for T
    xll_type_text<T>::value[Is]...,
    // recurse for the rest of the types
    xll_type_text_impl<Ts...>::value[Js]...,
    // final null terminator
    '\0'
  };
};

/**
 * Partial specialization that constructs a `pxTypeText` prototype.
 *
 * This contains a `constexpr const char[]` static `value` member, e.g. "BQQ",
 * the provides a *prototype* for `pxTypeText`. It is just a prototype because
 * we cannot determine from the function type information itself whether the
 * function is thread-safe and/or should be marked as volatile.
 *
 * @tparam R Function return type
 * @tparam Ts Function argument types
 */
template <typename R, typename... Ts>
struct xll_type_text<R(__stdcall *)(Ts...)> : xll_type_text_impl<R, Ts...> {
  static_assert(sizeof...(Ts) < 256, "max number of Excel 12 arguments is 255");
};

}  // namespace detail

/**
 * XLL add-in user-defined function (UDF) argument.
 *
 * This simply holds just the argument name and argument help text.
 */
class udf_arg {
public:
  /**
   * Ctor.
   *
   * @param name Argument name
   * @param help Argument help text
   */
  udf_arg(std::string name, std::string help) noexcept;

  /**
   * Return the argument name.
   */
  std::string_view name() const noexcept;

  /**
   * Return the argument help text.
   */
  std::string_view help() const noexcept;

private:
  std::string name_;  // parameter name (for pxArgumentText)
  std::string help_;  // parameter description (pxArgumentHelp[n])
};

/**
 * XLL add-in user-defined function (UDF).
 *
 * This class represents all the necessary state required for a call to
 * `xlfRegister` (Form 1) in order to register an exported DLL function as an
 * Excel add-in function or macro available for use in Excel.
 */
class udf {
public:
  using udf_args = std::vector<udf_arg>;

  /**
   * Ctor.
   *
   * This takes a given exported DLL function name and the function itself and
   * populates the exported DLL name, Excel name, and type text template. The
   * type text template is not exactly what is returned to Excel because it is
   * still up to the user to specify if the UDF is thread-safe or volatile.
   *
   * @tparam R Function return type
   * @tparam Ts Function argument types
   *
   * @param name Function name as exported from DLL
   * @param func Function exported from DLL
   */
  template <typename R, typename... Ts>
  udf(std::string name, R(__stdcall *func)(Ts...))
    : export_name_{std::move(name)}
  {
    type_template_ = detail::xll_type_text<decltype(func)>::value;
    name_ = export_name_;
  }

  /**
   * Return the name of the exported DLL function.
   *
   * This can no longer be changed after the `udf` is constructed.
   */
  std::string_view export_name() const noexcept;

  /**
   * Return the `pxTypeText` type text template.
   *
   * Use `type_text()` for the string that will be used as the `pxTypeText`
   * value as it incorporates the volatile and thread-safe qualifiers.
   */
  std::string_view type_template() const noexcept;

  /**
   * Return the `pxTypeText` string.
   *
   * This takes into account volatile and thread-safe qualifiers.
   */
  std::string type_text() const;

  /**
   * Return the `pxArgumentText` string.
   *
   * This is simply the argument names separated by commas.
   */
  std::string arg_text() const;

  /**
   * Update the name of the function as it appears in Excel.
   *
   * By default this is the same as `export_name()` but it can be different.
   *
   * @param text Excel function name
   */
  udf& name(std::string text) noexcept;

  /**
   * Return the name of the function as it appears in Excel.
   */
  std::string_view name() const noexcept;

  /**
   * Update the XLL function type.
   *
   * The only valid values are 0, 1, and 2, where 1 is for standard worksheet
   * functions, 0 is for a worksheet function not available in the Function
   * Wizard, and 2 is for macro sheet functions.
   *
   * If not specified the value is 1 by default.
   *
   * @todo Need to think a bit harder about how to do this.
   *
   * @param v Function type
   */
  udf& type(int v);

  /**
   * Return the XLL function type.
   */
  int type() const noexcept;

  /**
   * Update the XLL function category.
   *
   * @param text Function category
   */
  udf& category(std::string text) noexcept;

  /**
   * Return the XLL function category.
   *
   * By default, the category is empty, and the UDF goes to "User Defined".
   */
  std::string_view category() const noexcept;

  /**
   * Add an argument description to the UDF.
   *
   * @param name Argument name
   * @param help Argument help text
   */
  udf& arg(std::string name, std::string help);

  /**
   * Update the help topic for the UDF.
   *
   * @param text Help topic URL or file path
   */
  udf& help_topic(std::string text) noexcept;

  /**
   * Return the help topic for the UDF.
   *
   * By default the help topic is empty (not required).
   */
  std::string_view help_topic() const noexcept;

  /**
   * Update the UDF help text.
   *
   * This can be a multi-line string so as to provide appropriate detail.
   *
   * @param text Help text
   */
  udf& help(std::string text) noexcept;

  /**
   * Return the UDF help text.
   */
  std::string_view help() const noexcept;

  /**
   * Return the vector of UDF arguments.
   */
  const udf_args& args() const noexcept;

  /**
   * Mark the XLL UDF as volatile.
   *
   * If not specified the function is marked as non-volatile by default.
   *
   * @param v `true` to mark function as volatile
   */
  udf& volatile_(bool v) noexcept;

  /**
   * Indicate if the UDF is volatile or not.
   */
  bool volatile_() const noexcept;

  /**
   * Mark the XLL UDF as thread-safe.
   *
   * If not specified the function is marked as thread-safe by default.
   *
   * @param v `true` to mark function as thread-safe
   */
  udf& thread_safe(bool v) noexcept;

  /**
   * Indicate if the UDF is thread-safe or not.
   */
  bool thread_safe() const noexcept;

private:
  std::string export_name_;    // DLL export name (pxProcedure)
  std::string type_template_;  // [partial] type text template (pxTypeText)
  std::string name_;           // Excel function name (pxFunctionText)
  int type_{1};                // function/macro type (pxMacroType)
  std::string category_;       // function category (pxCategory)
  char shortcut_{};            // shortcut key (pxShortcutText)
  std::string help_topic_;     // help topic link (pxHelpTopic)
  std::string help_;           // function help text (pxFunctionHelp)
  udf_args args_;              // UDF arguments
  bool is_volatile_{};         // indicate if UDF is volatile
  bool thread_safe_{true};     // indicate if UDF is thread-safe
};

/**
 * XLL add-in user-defined function (UDF) registry.
 *
 * This provides a fluent interface for adding new UDFs and range-for support.
 */
class udf_registry {
public:
  using container = std::vector<udf>;
  using iterator = typename container::const_iterator;

  // TODO: document
  template <typename R, typename... Ts>
  auto& add(std::string name, R(__stdcall *func)(Ts...))
  {
    udfs_.emplace_back(name, func);
    return *this;
  }

  // TODO: document
  iterator begin() const;
  iterator end() const;
  udf& back();

private:
  container udfs_;
};

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_UDF_H_
