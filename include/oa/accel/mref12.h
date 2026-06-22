/**
 * @file mref12.h
 * @author Derek Huang
 * @brief C++ header for the Excel 12 multi-reference type
 * @copyright MIT License
 */

#ifndef OA_ACCEL_MREF12_H_
#define OA_ACCEL_MREF12_H_

// forward decl to avoid pulling in XLCALL.H
struct xlmref12;
struct xlref12;

#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iosfwd>

namespace oa {
namespace accel {

/**
 * Allocate a `xlmref12` using for a specified number of `xlref12`.
 *
 * This safely allocates a `XLMREF12` which can refer to more memory than its
 * `sizeof()` value implies, as the `XLMREF12` is of variable length:
 *
 * @code
 * +---------------------+
 * | WORD count;         |  // object count
 * | XLREF12 reftbl[1];  |  // first XLREF12 object
 * +---------------------+
 * | XLREF12[count - 1]; |  // subsequent XLREF12 objects
 * +---------------------+
 * @endcode
 *
 * We therefore manually allocate the right number of bytes, which will be
 * `sizeof(xlmref12)` plus `sizeof(xlref12) * (count - 1)`. Allocating the
 * `xlmref12` is done using placement `new` to satisfy C++ object construction
 * semantics to avoid any undefined behavior. The special case of `count` being
 * zero is handled by simply returning `nullptr`.
 *
 * Any allocated `xlmref12*` buffers should be freed with `xlmref12_free()`.
 *
 * @param count Number of `xlref12` objects to allocate space for
 */
xlmref12* xlmref12_malloc(std::uint16_t count);

/**
 * Allocate and zero a `xlmref12` for a given number of `xlref12`.
 *
 * This has the same semantics as `xlmref12_malloc()` but zeroes the memory.
 *
 * @param count Number of zeroed `xlref12` objects to allocate space for
 */
xlmref12 *xlmref12_calloc(std::uint16_t count);

/**
 * Create a copy of the `xlmref12`.
 *
 * The new `xlmref12` is allocated using `xlmref12_malloc()`. The count and
 * individual `xlre12` objects are simply copied using `memcpy()`.
 *
 * If `mref` is `nullptr`, then `nullptr` is simply returned.
 *
 * @param mref `xlmref12` to copy from
 */
xlmref12* xlmref12_copy(const xlmref12* mref);

/**
 * Frees the memory associated with the `xlmref12`.
 *
 * This explicitly calls the implicit `~xlmref12()` to satisfy C++ object
 * lifetime semantics and calls `delete[]` to free the allocated buffer. If
 * `mref` is `nullptr` then nothing will be done.
 *
 * @param mref `xlmref12` to free
 */
void xlmref12_free(xlmref12* mref) noexcept;

/**
 * Excel 12 multiple sheet reference management class.
 *
 * This provides a way to safely allocate and manipulate a `XLMREF12` which can
 * refer to more memory than its `sizeof()` value implies, as although the
 * `XLMREF12` has a fixed size, it describes a variable-length structure:
 *
 * @code
 * +---------------------+
 * | WORD count;         |  // object count
 * | XLREF12 reftbl[1];  |  // first XLREF12 object
 * +---------------------+
 * | XLREF12[count - 1]; |  // subsequent XLREF12 objects
 * +---------------------+
 * @endcode
 *
 * We get around this by manually allocating the right size and using placement
 * `new` to write the `XLMREF12` body with the `count` member set.
 *
 * This class satsifies some of the *Container* requirements.
 */
class mref12 {
public:
  // partial *Container* support for code that uses value_type instead of the
  // more comprehensive std::decay_t<decltype(std::begin(range))>
  using value_type = xlref12;

  /**
   * Default ctor.
   *
   * This has the same effect as `mref12(0u)`.
   */
  mref12() = default;

  /**
   * Copy ctor.
   */
  mref12(const mref12& other);

  /**
   * Copy assignment operator.
   */
  mref12& operator=(const mref12& other);

  /**
   * Move ctor.
   */
  mref12(mref12&& other) noexcept;

  /**
   * Move assignment operator.
   */
  mref12& operator=(mref12&& other) noexcept;

  /**
   * Dtor.
   *
   * Deallocates any memory that is managed by the class if necessary.
   */
  ~mref12();

  /**
   * Ctor.
   *
   * Allocates space for the requested number of `XLREF12` instances. The
   * maximum value cannot exceed the `WORD` maximum. No memory is zeroed.
   *
   * If `count` is zero no memory is allocated and the sheet ID is not set.
   * Otherwise, the sheet ID is obtained using `xlSheetId`.
   *
   * @note This function is typically not very useful by itself.
   *
   * @param count Number of `XLREF12` instances to allocate memory for
   */
  mref12(std::size_t count);

  /**
   * Ctor.
   *
   * List-initialize by copy using the specified `xlref12` objects. The sheet
   * ID is obtained by a call to `xlSheetId` unless the list is empty.
   *
   * @param refs Initializer list of `xlref12` objects
   */
  mref12(std::initializer_list<xlref12> refs);

  /**
   * Ctor.
   *
   * List-initialize by copy using the given `xlref12` objects and sheet ID. If
   * the initializer list is empty, however, then the sheet ID is not set.
   *
   * @param id Excel sheet ID from `xlSheetId`
   * @param refs Initializer list of `xlref12` objects
   */
  mref12(std::uintptr_t id, std::initializer_list<xlref12> refs);

  /**
   * Compare against another `mref12` for equality.
   *
   * Result is unspecified if any `xlref12` instances have unspecified values.
   *
   * @param other `mref12` to compare against
   */
  bool operator==(const mref12& other) const noexcept;

  /**
   * Return the Excel sheet ID associated with the reference.
   *
   * This is zero if the `mref12` is empty or an `xlSheetId` call failed, the
   * latter of which shouldn't happen if constructed while running in Excel.
   */
  std::uintptr_t sheet() const noexcept;

  /**
   * Release ownership of the `xlmref12`.
   *
   * This is mostly useful for interop with an `oper12` that will take
   * ownership. After `release()` is called the `mref12` has `nullptr` data.
   */
  xlmref12* release() noexcept;

  /**
   * Return a pointer to the first managed `xlref12`.
   *
   * If the `mref12` is empty `nullptr` is returned.
   */
  xlref12* data() noexcept;

  /**
   * Return a pointer to the first managed `xlref12`.
   *
   * If the `mref12` is empty `nullptr` is returned.
   */
  const xlref12* data() const noexcept;

  /**
   * Indicate if the `mref12` contains any `xlref12` instances.
   */
  bool empty() const noexcept;

  /**
   * Return the number of managed `xlref12` structures.
   *
   * If `size()` returns zero no `xlref12` structures are managed.
   */
  std::size_t size() const noexcept;

  /**
   * Return a reference to the `i`th `xlref12`.
   *
   * If the `mref12` is empty calling `operator[]` is undefined behavior.
   *
   * @param i Index to the `i`th `xlref12`
   */
  xlref12& operator[](std::size_t i) noexcept;

  /**
   * Return a reference to the `i`th `xlref12`.
   *
   * If the `mref12` is empty calling `operator[]` is undefined behavior.
   *
   * @param i Index to the `i`th `xlref12`.
   */
  const xlref12& operator[](std::size_t i) const noexcept;

  /**
   * Return a pointer to the first managed `xlref12`.
   *
   * If the `mref12` is empty `nullptr` is returned.
   */
  xlref12* begin() noexcept;

  /**
   * Return a pointer to the first managed `xlref12`.
   *
   * If the `mref12` is empty `nullptr` is returned.
   */
  const xlref12* begin() const noexcept;

  /**
   * Return a pointer to one past the last managed `xlref12`.
   *
   * If the `mref12` is empty `nullptr` is returned.
   */
  xlref12* end() noexcept;

  /**
   * Return a pointer to one past the last managed `xlref12`.
   *
   * If the `mref12` is empty `nullptr` is returned.
   */
  const xlref12* end() const noexcept;

private:
  std::uintptr_t sheet_{};  // Excel sheet ID from xlSheetId
  xlmref12* value_{};       // xlmref12 data pointer

  /**
   * Set sheet ID and allocate memory for the given number of `xlref12`.
   *
   * If `count` exceeds `std::numeric_limits<WORD>::max()` this will throw.
   *
   * @note Only needs to be called when `count` is nonzero.
   *
   * @param id Excel sheet ID
   * @param count Number of `xlref12` to allocate for
   */
  void init(std::uintptr_t id, std::size_t count);

  /**
   * Set sheet ID and initialize from the given `xlref12.
   *
   * If `count` exceeds `std::numeric_limits<WORD>::max()` this will throw.
   *
   * @note Only needs to be called when `count` is nonzero.
   *
   * @param id Excel sheet ID
   * @param refs Initializer list of `xlref12` objects.
   */
  void init(std::uintptr_t id, std::initializer_list<xlref12> refs);

  /**
   * Initialize by copy from a `mref12`.
   *
   * The `xlref12` copy is implemented using `xlmref12_copy()`.
   */
  void from(const mref12& other);

  /**
   * Initialize by move from a `mref12`.
   *
   * On completion, the moved-from `mref12` has zeroed data pointer and ID.
   */
  void from(mref12&& other) noexcept;

  /**
   * Deallocate any memory if the `xlmref12*` is not `nullptr`.
   *
   * This is implemented using `xlmref12_free()`.
   */
  void destroy() noexcept;
};

/**
 * Stream operator for the `mref12`.
 *
 * This streams each `xlref12` using `operator<<` between delimiters:
 *
 * @code
 * <hex sheet ID>: [(r1, c1), (r2, c2) ... (r3, c3), ... ]
 * @endcode
 *
 * If the `mref12` is empty then just `"<hex sheet ID>: []"` will be formatted.
 *
 * @param out Output stream
 * @param ref Multi-reference to stream
 */
std::ostream& operator<<(std::ostream& out, const mref12& ref);

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_MREF12_H_
