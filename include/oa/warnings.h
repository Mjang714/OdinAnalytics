/**
 * @file warnings.h
 * @author Derek Huang
 * @brief C/C++ header for warning macros definitions
 * @copyright MIT License
 */

#ifndef OA_WARNINGS_H_
#define OA_WARNINGS_H_

// for OA_CONCAT(), OA_STRINGIFY()
#include "oa/common.h"

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
 * This uses multiple `OA_STRINGIFY()` calls to correctly quote arguments.
 *
 * @param wname Warning name, e.g. maybe-unitialized, without quotes
 */
#define OA_GNU_WARNING_DISABLE(wname) \
  _Pragma(OA_STRINGIFY(GCC diagnostic ignored OA_STRINGIFY(OA_CONCAT(-W, wname))))

/**
 * Pop GNU warning state.
 */
#define OA_GNU_WARNING_POP() _Pragma("GCC diagnostic pop")
#else
#define OA_GNU_WARNING_PUSH()
#define OA_GNU_WARNING_DISABLE(wname)
#define OA_GNU_WARNING_POP()
#endif  // __GNUC__

// internal macro for emitting a preprocessing message with file, line number,
// and a message prefix. this implements OA_MESSAGE(), OA_WARNING() for MSVC
#define OA_CUSTOM_MESSAGE_(pre, msg) \
  _Pragma(OA_STRINGIFY(message( \
    __FILE__ ":" OA_STRINGIFY(__LINE__) ": " pre ": " msg \
  )))

#if defined(_MSC_VER)
/**
 * Emit an informational message during preprocessing.
 *
 * To better emulate the GCC `#pragma message` output the file, line number,
 * and a `message: ` prefix are all prepended to the message.
 *
 * @param msg Message string literal
 */
#define OA_MESSAGE(msg) OA_CUSTOM_MESSAGE_("message", msg)

/**
 * Emit a warning message during preprocessing.
 *
 * To better emulate the GCC `#pragma GCC warning` output the file, line number,
 * and a `warning: ` prefix are all prepended to the message.
 *
 * @param msg Message string literal
 */
#define OA_WARNING(msg) OA_CUSTOM_MESSAGE_("warning", msg)
#elif defined(__GNUC__)
/**
 * Emit an informational message during preprocessing.
 *
 * @param msg Message string literal
 */
#define OA_MESSAGE(msg) _Pragma(OA_STRINGIFY(message msg))

/**
 * Emit a warning message during preprocessing.
 *
 * @param msg Message string literal
 */
#define OA_WARNING(msg) _Pragma(OA_STRINGIFY(GCC warning msg))
#else
#define OA_MESSAGE(msg)
#define OA_WARNING(msg)
#endif  // !defined(_MSC_VER) && !defined(__GNUC__)

#endif  // OA_WARNINGS_H_
