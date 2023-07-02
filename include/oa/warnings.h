/**
 * @file warnings.h
 * @author Derek Huang
 * @brief C/C++ header for warning macros definitions
 * @copyright MIT License
 */

#ifndef OA_WARNINGS_H_
#define OA_WARNINGS_H_

#ifdef _MSC_VER
/**
 * Push MSVC warning state.
 */
#define OA_MSVC_WARNING_PUSH() __pragma(warning(push))

/**
 * Disable specified MSVC warnings.
 *
 * @param wnos Warning number(s), e.g. 4242
 */
#define OA_MSVC_WARNING_DISABLE(wnos) __pragma(warning(disable: wnos))
/**
 * Pop MSVC warning state.
 */
#define OA_MSVC_WARNING_POP() __pragma(warning(pop))
// do nothing when not compiling with MSVC
#else
#define OA_MSVC_WARNING_PUSH()
#define OA_MSVC_WARNING_DISABLE(wnos)
#define OA_MSVC_WARNING_POP()
#endif  // _MSC_VER

// Clang also defines __GNUC__, hence it is "GNU warning", not "GCC warning"
#ifdef __GNUC__
/**
 * Push GNU warning state.
 */
#define OA_GNU_WARNING_PUSH() _Pragma("GCC diagnostic push")

/**
 * Disable specified GNU warning.
 *
 * @param wname Warning name, e.g. maybe-unitialized
 */
#define OA_GNU_WARNING_DISABLE(wname) \
  _Pragma("GCC diagnostic warning \"-W" #wname "\"")

/**
 * Pop GNU warning state.
 */
#define OA_GNU_WARNING_POP() _Pragma("GCC diagnostic pop")
#else
#define OA_GNU_WARNING_PUSH()
#define OA_GNU_WARNING_DISABLE(wname)
#define OA_GNU_WARNING_POP()
#endif  // __GNUC__

#endif  // OA_WARNINGS_H_
