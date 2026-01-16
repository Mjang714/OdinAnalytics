/**
 * @file accel/addin.h
 * @author Derek Huang
 * @brief C++ header for an XLL add-in framework class
 * @copyright MIT License
 */

#ifndef OA_ACCEL_ADDIN_H_
#define OA_ACCEL_ADDIN_H_

#include <string>
#include <string_view>

// forward decl to avoid pulling in XLCALL.H
struct xloper12;

namespace oa {
namespace accel {

/**
 * Macro for exporting an XLL function.
 *
 * Functions exported from a Win32 XLL have callee-cleaned stacks and so must
 * use the `WINAPI`, i.e. `__stdcall`, calling convention. For example:
 *
 * @code{.cc}
 * OA_XLL_EXPORT(double) MyFunc(long v)
 * {
 *   // ...
 * }
 * @endcode
 *
 * @todo Replace `WINAPI` with `__stdcall` directly as it is not 2005 anymore.
 *
 * @param ret Function return type
 */
#define OA_XLL_EXPORT(ret) __declspec(dllexport) ret WINAPI

/**
 * Macro to mark the beginning of a function try-block.
 *
 * This can be used with some of the other Accel macros to simplify the work of
 * making exported XLL functions exception safe:
 *
 * @code{.cc}
 * OA_XLL_EXPORT(xloper12*) HelloWorld() OA_ACCEL_SAFE(noexcept)
 * {
 *   oper12 res{"hello world"};
 *   OA_ACCEL_SAFE_RETURN(res.release()));
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
 * Include `<exception>` and `oper12.h` before using this macro.
 *
 * @note If `expr` is multi-line or has control flow consider using a lambda
 *  for `expr` to enable wrapping of the relevant logic.
 *
 * @param expr Expression to return when no exception is thrown
 */
#define OA_ACCEL_SAFE_RETURN(expr) \
    return (expr); \
  } \
  catch (const std::exception& exc) { \
    return oa::accel::oper12{exc.what()}.release(); \
  } \
  catch (...) { \
    return oa::accel::oper12{"Unknown C++ exception"}.release()

/**
 * Class representing the overall state of an implemented XLL add-in.
 *
 * This *must* be subclassed and *one* static instance constructed by every XLL
 * using the Accel framework. Any state that should be scoped to an add-in's
 * lifetime can be added to subclasses as an abstraction.
 *
 * Virtual functions for customization are provided.
 */
class addin {
public:
  /**
   * Deleted copy ctor.
   *
   * We don't support a notion of "copying" an add-in around.
   */
  addin(const addin&) = delete;

  /**
   * Return the singleton add-in instance loaded by Excel.
   *
   * A reference to a default-constructed `addin` is returned if no
   * user-defined add-in instances have been registered.
   */
  static addin& instance();

  /**
   * Obtain the full path to the loaded XLL.
   *
   * The path can be used by `GetModuleHandleA()` to obtain the XLL `HMODULE`.
   *
   * @note This uses `xlfGetName` and so can only be used when XLL is loaded.
   */
  static std::string_view path();

  /**
   * Obtain the full path to the loaded XLL.
   *
   * Ths path can be used by `GetModuleHandleW()` to obtain the XLL `HMODULE`.
   *
   * @note This uses `xlfGetName` and so can only be used when XLL is loaded.
   */
  static std::wstring_view wpath();

  /**
   * Return the registered XLL add-in long name displayed in the add-in menu.
   *
   * The string returned by this function will be returned by the Accel
   * `xlAddInManagerInfo12()` implementation. It should contain the name of the
   * add-in and optionally a version identifier. What is returned will shown as
   * an entry in the Excel dialog that displays the list of available add-ins.
   *
   * If not implemented `"<xll name>.xll dev"` is returned as the default.
   */
  virtual std::string long_name() const;

  /**
   * `xlAutoFree12()` action run before an XLL-allocated `XLOPER12` is freed.
   *
   * This provides the add-in with an opportunity to perform logging or other
   * actions using other information about the `XLOPER12` being freed.
   *
   * @todo Some kind of C++ `XLOPER12` view class would simplify things. Also
   *  consider using a private virtual pattern for exception handling.
   *
   * @param op `XLOPER12` to be freed
   */
  virtual void on_auto_free(const xloper12* op);

protected:
  /**
   * Default ctor.
   *
   * This is protected as users are not allowed to construct base instances.
   */
  addin();

  /**
   * Dtor.
   *
   * This is protected to prevent manual destruction through base pointers.
   */
  virtual ~addin();
};

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_ADDIN_H_
