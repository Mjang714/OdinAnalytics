/**
 * @file accel/addin.h
 * @author Derek Huang
 * @brief C++ header for an XLL add-in framework class
 * @copyright MIT License
 */

#ifndef OA_ACCEL_ADDIN_H_
#define OA_ACCEL_ADDIN_H_

#include <exception>
#include <functional>
#include <string>
#include <string_view>

#include "oa/accel/oper12.h"
#include "oa/accel/menu.h"
#include "oa/accel/udf.h"     // for udf_registry
#include "oa/common.h"        // for OA_CONCAT()

// forward decl to avoid pulling in XLCALL.H
struct xloper12;

namespace oa {
namespace accel {

namespace detail {

/**
 * Type alias providing the `OA_XLL_EXPORT()` default return type.
 *
 * @tparam T type
 */
template <typename T = xloper12*>
using udf_return_t = T;

}  // namespace detail

/**
 * Macro for exporting an XLL function.
 *
 * Functions exported from a Win32 XLL have callee-cleaned stacks and so must
 * use the `WINAPI`, i.e. `__stdcall`, calling convention. For example:
 *
 * @code{.cc}
 * OA_XLL_EXPORT() AddOne(double v) OA_ACCEL_SAFE()
 * {
 *   OA_ACCEL_SAFE_RETURN(v + 1);
 * }
 * @endcode
 *
 * If no arguments are provided then the return type is implicitly `xloper12*`
 * for ease of integration with `OA_ACCEL_SAFE_RETURN()`. In this case, we do
 * not currently have `OA_ACCEL_SAFE_RETURN()` integration:
 *
 * @code{.cc}
 * OA_XLL_EXPORT(double) AddOneRaw(double v) noexcept
 * {
 *   return v + 1;
 * }
 * @endcode
 *
 * @param ... Function return type
 */
#define OA_XLL_EXPORT(...) \
  extern "C" __declspec(dllexport) \
  oa::accel::detail::udf_return_t<__VA_ARGS__> __stdcall

/**
 * Macro to mark the beginning of a function try-block.
 *
 * This can be used with some of the other Accel macros to simplify the work of
 * making exported XLL functions exception safe:
 *
 * @code{.cc}
 * OA_XLL_EXPORT() HelloWorld() OA_ACCEL_SAFE(noexcept)
 * {
 *   OA_ACCEL_SAFE_RETURN("hello world");
 * }
 * @endcode
 */
#define OA_ACCEL_SAFE(...) __VA_ARGS__ try

/**
 * Macro to mark a `catch` block for exception handling logic.
 *
 * @param expr `catch` expression specifier
 */
#define OA_ACCEL_CATCH(expr) } catch (expr) {

/**
 * Macro for use in XLL functions that handles C++ exceptions as strings.
 *
 * The exception message will be returned back to Excel so any XLL function
 * using this macro must return an `LPXLOPER12`.
 *
 * @note If `expr` is multi-line or has control flow consider using a lambda
 *  for `expr` to enable wrapping of the relevant logic.
 *
 * @par
 *
 * @note We purposefully use list-initialization with the `oper12` to disallow
 *  some dangerous narrowing conversions, e.g. pointer to `bool`.
 *
 * @param expr Expression to list-initialize the `oper12` to return
 */
#define OA_ACCEL_SAFE_RETURN(expr) \
    return oa::accel::oper12{expr}.release(); \
  } \
  catch (const std::exception& exc) { \
    return oa::accel::oper12{std::string{"#ERROR: "} + exc.what()}.release(); \
  } \
  catch (...) { \
    return oa::accel::oper12{"#ERROR: Unknown C++ exception"}.release()

/**
 * Class representing the overall state of an implemented XLL add-in.
 *
 * Instead of taking the XLL+ approach where users subclass the base add-in
 * class, this type provides type-erasure hooks using `std::function<>` that
 * provide ways to customize add-in behavior during static object construction.
 *
 * A fluent API is provided that presents a declarative feel for users.
 */
class addin {
public:
  using xloper12_callback = std::function<void(const xloper12&)>;

  /**
   * Deleted copy ctor.
   *
   * We don't support a notion of "copying" an add-in around.
   */
  addin(const addin&) = delete;

  /**
   * Return the singleton add-in instance loaded by Excel.
   */
  static addin& instance();

  /**
   * Return the UDF registry with all registered UDFs.
   */
  static udf_registry& udfs();

  /**
   * Return the add-in menu containing the menu items.
   */
  static menu& menu();

  /**
   * Obtain the full path to the loaded XLL.
   *
   * The path can be used by `GetModuleHandleA()` to obtain the XLL `HMODULE`.
   *
   * @note This uses `xlfGetName` and so must only be used if Excel is running.
   */
  static std::string_view path();

  /**
   * Obtain the full path to the loaded XLL.
   *
   * Ths path can be used by `GetModuleHandleW()` to obtain the XLL `HMODULE`.
   *
   * @note This uses `xlfGetName` and so must only be used if Excel is running.
   */
  static std::wstring_view wpath();

  /**
   * Obtain the filename of the loaded XLL.
   *
   * @note This uses `xlfGetName` and so must only be used if Excel is running.
   */
  static std::string_view filename();

  /**
   * Obtain the filename stem of the loaded XLL.
   *
   * This is the filename of the XLL without its trailing extension.
   *
   * @note This uses `xlfGetName` and so must only be used if Excel is running.
   */
  static std::string_view stem();

  /**
   * Update the XLL name displayed in the add-in menu returned by `name()`.
   *
   * Typically it is suggested to set this to the add-in's human-readable name
   * and some brief version information. It is important not to make the name
   * string too long, despite the moniker of "long name", as otherwise the text
   * will not be fully visible in the Excel add-in manager window.
   *
   * @param str Value to use for `name()`
   */
  addin& name(std::string str) noexcept;

  /**
   * Return the registered XLL add-in name displayed in the add-in menu.
   *
   * The string returned by this function will be returned to Excel by Accel's
   * `xlAddInManagerInfo12()` implementation. It should contain the name of the
   * add-in and optionally a version identifier. What is returned will show as
   * an entry in the Excel dialog that displays the list of available add-ins.
   *
   * If not set by the user `"<xll name>.xll"` is returned as the default when
   * invoked in Excel XLL interface functions. Therefore, do not call this
   * overload before Excel is running unless you already set `name()`.
   */
  std::string_view name() const;

  /**
   * Set the action run before freeing an XLL-allocated `XLOPER12`.
   *
   * This provides the add-in with an opportunity to perform logging or other
   * actions when `xlAutoFree12()` runs using information about the `XLOPER12`
   * that will be subsequently freed by the Accel add-in.
   *
   * If this is not set by the user a lambda with an empty body is run.
   *
   * @todo Some kind of C++ `XLOPER12` view class would simplify things.
   *
   * @param func Callable to invoke in `xlAutoFree12()` before `XLOPER12` free
   */
  addin& on_auto_free(xloper12_callback func);

  /**
   * Run the `xlAutoFree12()` action before the `XLOPER12` is deleted.
   *
   * @param op `XLOPER12` to inspect
   */
  void on_auto_free(const xloper12& op) const;

private:
  /**
   * Default ctor.
   *
   * This is private as end users are not allowed to construct instances.
   */
  addin();

  std::string name_;
  xloper12_callback on_auto_free_;
};

/**
 * Macro for instantiating the `addin` singleton and modifying its fields.
 *
 * This uses the `__LINE__` macro to ensure multiple uses in a translation unit
 * are unique and `static` scopes the reference to the translation unit.
 */
#define OA_ACCEL_ADDIN_INSTANCE() \
  static auto& OA_CONCAT(oa_accel_addin_, __LINE__) = oa::accel::addin::instance()

/**
 * Register a standard UDF using an exported XLL function.
 *
 * Method chaining can be used to set properties of the exported UDF.
 *
 * @param func Exported XLL function to register
 */
#define OA_ACCEL_EXPORT_FUNC(func) \
  static auto& OA_CONCAT(oa_accel_udf_ref_, __LINE__) = oa::accel::addin::udfs() \
    .add(OA_STRINGIFY(func), func) \
    /* to obtain a udf reference we use back() */ \
    .back()

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_ADDIN_H_
