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
#include <iosfwd>
#include <optional>
#include <string>
#include <string_view>

#include "oa/accel/enums.h"

// forward decl to avoid pulling in XLCALL.H
struct xloper12;
struct xlref12;

namespace oa {
namespace accel {

// forward decl
class mref12;

/**
 * Create a copy of the `XLOPER12` allocated with `new`.
 *
 * This calls `new` to allocate another `XLOPER12`, allocating any additional
 * memory required for the type, e.g. for `xltypeStr`, `xltypeRef`, or
 * `xltypeMulti` members. If `oper` is `nullptr`, `nullptr` is returned.
 *
 * @note This function discards the `xlbitDLLFree` and `xlbitXLFree` bits in
 *  `op->xltype` when copying this field to the new `XLOPER12` type member.
 *  This ensures that these deallocation flags are not falsely propagated.
 *
 * @warning It is incredibly easy to leak memory with this function in the
 *  presence of exceptions. Although the function itself satisfies the strong
 *  exception guarantee, the returned `xloper12*` may be associated with
 *  auxiliary memory that will be leaked should an exception be thrown after
 *  this function is called, e.g. if `xltypeStr`, `xltypeMulti`, `xltypeRef`.
 *
 * @param op `XLOPER12` to copy
 */
xloper12* xloper12_copy(const xloper12* op);

/**
 * Delete an `XLOPER12` allocated with `new` in an `oper12`.
 *
 * This deallocates any extra memory allocated for the `XLOPER12`, e.g. for
 * `xltypeStr`, `xltypeRef`, or `xltypeMulti` members, before calling `delete`
 * on the `XLOPER12` pointer itself. If `oper` is `nullptr`, nothing is done.
 *
 * @note This function correctly masks off `xlbitDLLFree`. Note that it is
 *  still an error to pass in an `XLOPER12` that has been allocated by Excel,
 *  even with the `xlbitXLFree` bit set, as `delete` is called on `op`.
 *
 * @param op `XLOPER12` to delete
 */
void xloper12_free(xloper12* op) noexcept;

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
   */
  oper12(const oper12&);

  /**
   * Copy assignment operator.
   */
  oper12& operator=(const oper12& other);

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
   * Constructs a string `XLOPER12` by copying from a null-terminated string.
   *
   * @param str Null-terminated buffer to copy from
   */
  oper12(const char* str);

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
   * Constructs a single-cell, single-reference `XLOPER12`.
   *
   * @param row Row index
   * @param col Column index
   */
  oper12(std::int32_t row, std::int32_t col);

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
   * Ctor.
   *
   * Constructs a `xltypeBigData` input `XLOPER12 using the given buffer
   * pointer and size. No copying of the buffer data is done.
   *
   * @param data Data buffer
   * @param size Data buffer length in bytes > 0
   */
  oper12(const char* data, std::size_t size);

  /**
   * Ctor.
   *
   * Constructs a `xltypeBigData` input `XLOPER12 using the given buffer
   * pointer and size. No copying of the buffer data is done.
   *
   * @param data Data buffer
   * @param size Data buffer length in bytes > 0
   */
  oper12(const unsigned char* data, std::size_t size);

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
   * Indicate if the `oper12` owns a value.
   *
   * This returns `true` when the `value_` pointer is not `nullptr`.
   */
  operator bool() const noexcept;

  /**
   * Indicate if the `oper12` is responsible for allocated `XLOPER12` data.
   *
   * Some `XLOPER12` data, e.g. for strings when type is `xltypeStr`, requires
   * additional memory allocation. All `oper12` value constructors will
   * appropriately set the ownership flag to `true` if necessary.
   *
   * If the ownership flag is `false`, for `XLOPER12` types like `xltypeStr`,
   * `xlTypeMulti`, and `xlTypeRef`, the `oper12` assumes the `XLOPER12` data
   * was allocated by Excel, so `Excel12(xlFree, ...)` will be called in dtor.
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

  // TODO: add as<T>() template for conversion to C++ types (converting allowed)

private:
  xloper12* value_{};  // heap-allocated XLOPER12
  bool owning_{};      // indicate if XLOPER12 memory is owned by oper12

  /**
   * Initialize from another `oper12` by copy.
   *
   * Copying the `XLOPER12` is done using `xloper12_copy()`. The new `oper12`
   * will determine its ownership by checking the `XLOPER12` type member.
   */
  void from(const oper12& other);

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

/**
 * Stream the `oper12` value to an output stream.
 *
 * The formatted value will look something like the following:
 *
 * @code
 * [owning=(true|false)] ...
 * @endcode
 *
 * The object formatting is implemented using the `operator<<` for the
 * `xloper12`. If the `oper12` doesn't manage a `xloper12`, e.g. because it has
 * been default-constructed or was moved from, the object representation of
 * the `oper12` is simply `"[owning=false] (empty)"`.
 *
 * @param out Output stream
 * @param op `oper12` value to write
 */
std::ostream& operator<<(std::ostream& out, const oper12& op);

/**
 * Return a string representation for the `oper12` value.
 *
 * This provides the same representation as that given by `operator<<
 *
 * @param op `oper12` value
 */
std::string to_string(const oper12& op);

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_OPER12_H_
