/**
 * @file accel/oper12.h
 * @author Derek Huang
 * @brief C++ header for the Excel 12 fundamental data type (operand)
 * @copyright MIT License
 */

#ifndef OA_ACCEL_OPER12_H_
#define OA_ACCEL_OPER12_H_

#include <concepts>
#include <cstdint>
#include <optional>
#include <string_view>

// forward decl to avoid pulling in XLCALL.H
struct xloper12;
struct xlref12;

namespace oa {
namespace accel {

// forward decl
class mref12;

/**
 * `XLOPER` and `XLOPER12` data types.
 *
 * Each member has the same value as the actual Excel data type integer.
 */
enum class xltype : int {
  num = 0x0001,      // xltypeNum
  str = 0x0002,      // xltypeStr
  bool_ = 0x0004,    // xltypeBool
  ref = 0x0008,      // xltypeRef
  err = 0x0010,      // xltypeErr
  flow = 0x0020,     // xltypeFlow
  multi = 0x0040,    // xltypeMulti
  missing = 0x0080,  // xltypeMissing
  nil = 0x0100,      // xltypeNil
  sref = 0x0400,     // xltypeSRef
  int_ = 0x0800,     // xltypeInt
  bigdata = 0x0802   // xltypeBigData
};

/**
 * Excel error codes.
 *
 * Each member has the same value as the actual Excel error code integer.
 */
enum class xlerr : int {
  null = 0,          // xlerrNull
  div0 = 7,          // xlerrDiv0
  value = 15,        // xkerrValue
  ref = 23,          // xlerrRef
  name = 29,         // xlerrName
  num = 36,          // xlerrNum
  na = 42,           // xlerrNA
  getting_data = 43  // xlerrGettingData
};

/**
 * Returns `true` if the given type enumeration requires owning extra memory.
 *
 * String, multi-cell reference, and array types have extra memory requirements
 * besides the `XLOPER12` allocation itself.
 */
constexpr bool needs_extra_memory(xltype type) noexcept
{
  switch (type) {
  case xltype::str:
  case xltype::ref:
  case xltype::multi:
    return true;
  default:
    return false;
  }
}

/**
 * Excel 12 fundamental data type "operand" management class.
 *
 * This provides the general cell operand type that can be used to manage
 * `XLOPER12` memory while still providing interop with Excel SDK C functions.
 *
 * For safety the internal `XLOPER12` will have all fields not explicitly
 * initialized zeroed out. This is important for `Excel12()` interop, as some
 * functions, e.g. `xlfGetBinaryName`, may not provide any way to distinguish
 * from their variant type, e.g. the bigdata union is not tagged, what member
 * has been modified. For the bigadta, with a zeroed `cbData` member, if the
 * `oper12` is of type `bigdata`, we know the union should contains a `HANDLE`,
 * not a `BYTE*` as when used with `xlDefineBinaryName`.
 */
class oper12 {
public:
  /**
   * Default ctor.
   *
   * Constructs an empty `oper12` that does not own an `XLOPER12`.
   */
  oper12() noexcept = default;

  /**
   * Copy ctor.
   *
   * @todo Currently deleted for simplicity. We will support copying later.
   */
  oper12(const oper12&) = delete;

  /**
   * Move ctor.
   */
  oper12(oper12&& other) noexcept;

  /**
   * Move assignment operator.
   */
  oper12& operator=(oper12&& other) noexcept;

  /**
   * Dtor.
   *
   * This calls `destroy()` to free the `XLOPER12` memory as appropriate.
   */
  ~oper12();

  /**
   * Ctor.
   *
   * Constructs an error value from the given `xlerr` enumeration.
   *
   * @param err Excel error value
   */
  oper12(xlerr err);

  /**
   * Ctor.
   *
   * Constructs a boolean `XLOPER12` value.
   *
   * @param val Boolean value
   */
  oper12(bool val);

  /**
   * Ctor.
   *
   * Constructs an integral `XLOPER12` value.
   *
   * @param val Integer value
   */
  oper12(int val);

  /**
   * Ctor.
   *
   * Constructs a numeric `XLOPER12` value.
   *
   * @param val Floating value to store
   */
  oper12(double val);

  /**
   * Ctor.
   *
   * Constructs a string `XLOPER12` by copying from a narrow string view.
   *
   * @param str String view to construct from
   */
  oper12(std::string_view str);

  /**
   * Ctor.
   *
   * Constructs a string `XLOPER12` by copying from a wide string view.
   *
   * @param str String view to construct from
   */
  oper12(std::wstring_view str);

  /**
   * Ctor.
   *
   * Constructs a single-reference `XLOPER12`.
   *
   * @param ref Reference to use
   */
  oper12(xlref12 ref);

  /**
   * Ctor.
   *
   * Constructs a multi-reference `XLOPER12` by copying from a `mref12`.
   *
   * @param mref Multi-reference object to copy from
   */
  oper12(const mref12& mref);

  /**
   * Ctor.
   *
   * Constructs a multi-reference `XLOPER12` by moving from a `mref12`.
   *
   * @param mref Multi-reference object to move from
   */
  oper12(mref12&& mref);

  /**
   * Return a `oper12` owning an `XLOPER12` of type `xltypeNil`.
   *
   * It is recommended to use this or `missing()` for Excel C function interop.
   */
  static oper12 nil();

  /**
   * Return a `oper12` owning an `XLOPER12` of type `xltypeMissing`.
   *
   * It is recommended to use this or `nil()` for Excel C function interop.
   */
  static oper12 missing();

  /**
   * Return the `XLOPER12` pointer owned by the object.
   *
   * This is useful for C function interop but can be abused.
   *
   * @todo Once we have a good `to<T>()` implementation this may be removed.
   *  We may also implement a `Excel12()` wrapper for safe hydration.
   */
  xloper12* value() noexcept;

  /**
   * Return a const-qualified version of the owned `XLOPER12` pointer.
   *
   * This prevents calling a C function that modifies the `XLOPER12` on
   * `value()` when the `oper12` is const-qualified.
   */
  const xloper12* value() const noexcept;

  /**
   * Indicate if the `oper12` is responsible for allocated `XLOPER12` data.
   *
   * Some `XLOPER12` data, e.g. for strings when type is `xltypeStr`, requires
   * additional memory allocation. All `oper12` value constructors will
   * appropriately set the ownership flag to `true` if necessary.
   *
   * If the ownership flag is `false`, for `XLOPER12` types like `xltypeStr`,
   * `xlTypeMulti`, and `xlTypeRef`, the `oper12` assumes the `XLOPER12` data
   * was allocated by Excel, and so `Excel12(xlFree, ...)` is called.
   */
  bool owning() const noexcept;

  /**
   * Release ownership of the allocated `XLOPER12` back to Excel.
   *
   * After this call, the `oper12` will have no owned value. If `owning()`
   * returns `true`, then `xlbitDLLFree` will be set. Otherwise, `xlbitXLFree`
   * will be set in the `XLOPER12` if the type is `xltypeStr`, `xltypeMulti`,
   * or `xltypeRef`, all of which have extra memory to free.
   *
   * @warning Do not use the released `XLOPER12` with Excel C API functions or
   *  any other functions that do not mask `xlbitDLLFree` and `xlbitXLFree`
   *  before checking the value of the Excel type. Otherwise, these functions
   *  will get confused, and may behave unexpectedly.
   */
  xloper12* release() noexcept;

  /**
   * Return the type enumeration corresponding to the `XLOPER12` type.
   */
  xltype type() const noexcept;

  /**
   * Return the `XLOPER12` error value if `type()` is `xltype::err`.
   *
   * If the `XLOPER12` does not contain an error the optional is empty.
   */
  std::optional<xlerr> error() const noexcept;

  // TODO: add to<T>() template for conversion to C++ types (converting allowed)

private:
  xloper12* value_{};  // heap-allocated XLOPER12
  bool owning_{};      // indicate if XLOPER12 memory is owned by oper12

  /**
   * Initialize from another `oper12` by move.
   *
   * After this function is called the moved-from `oper12` is completely zeroed,
   * i.e. its data is `nullptr` and it owns no extra `XLOPER12` memory.
   */
  void from(oper12&& other) noexcept;

  /**
   * Deallocate the managed `XLOPER12` if necessary.
   *
   * If the `XLOPER12` pointer is not `nullptr`, the `XLOPER12` will be deleted
   * using `delete`. If `owning()` is `true`, then appropriate steps are taken
   * to free the `XLOPER12` data depending on its type. Otherwise, for the
   * `xltypeStr`, `xltypeMulti`, and `xltypeRef` types, it is assumed the
   * `XLOPER12` data is from Excel, so `Excel12(xlFree, ...)` is called.
   */
  void destroy() noexcept;
};

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_OPER12_H_
