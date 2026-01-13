/**
 * @file accel/enums.h
 * @author Derek Huang
 * @brief C++ header for Excel type, error, return enumerations
 * @copyright MIT License
 */

#ifndef OA_ACCEL_ENUMS_H_
#define OA_ACCEL_ENUMS_H_

namespace oa {
namespace accel {

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

// TODO: add xlflow data enums

/**
 * `Excel12()` and `Excel12v()` return value enum.
 *
 * Each member has the same value as the actual Excel return value integer.
 */
enum class xlret : int {
  success = 0,              // xlretSuccess
  abort = 1,                // xlretAbort
  inv_xlfn = 2,             // xlretInvXlfn
  inv_count = 4,            // xlretInvCount
  inv_xloper = 8,           // xlretInvXloper
  stack_ovfl = 16,          // xlretStackOvfl
  failed = 32,              // xlretFailed
  uncalced = 64,            // xlretUncalced
  not_thread_safe = 128,    // xlretNotThreadSafe
  inv_async_context = 256,  // xlretInvAsnychronousContext
  not_cluster_safe = 512    // xlretNotClusterSafe
};

/**
 * XLL event codes.
 *
 * Each member has the same value as the corresponding `xlevent*` macro.
 */
enum class xlevent : int {
  calc_end = 1,    // xleventCalculationEnded
  calc_cancel = 2  // xleventCalculationCanceled
};

/**
 * Returns `true` if the given type enumeration requires owning extra memory.
 *
 * String, multi-cell reference, and array types have extra memory requirements
 * besides the `XLOPER12` allocation itself.
 */
constexpr bool needs_aux_memory(xltype type) noexcept
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

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_ENUMS_H_
