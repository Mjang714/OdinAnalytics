/**
 * @file common.h
 * @author Derek Huang
 * @brief C/C++ header for common macros and helpers
 * @copyright MIT License
 */

#ifndef OA_COMMON_H_
#define OA_COMMON_H_

// macro for C++ version value avoiding the need for /Zc:__cplusplus
#if defined(_MSVC_LANG)
#define OA_CPLUSPLUS _MSVC_LANG
#elif defined(__cplusplus)
#define OA_CPLUSPLUS __cplusplus
#endif  // !defined(_MSVC_LANG) && !defined(__cplusplus)

/**
 * Stringify without macro expansion.
 *
 * @param x Value to stringify
 */
#define OA_STRINGIFY_I(x) #x

/**
 * Stringify with macro expansion.
 *
 * @param x Value to stringify
 */
#define OA_STRINGIFY(x) OA_STRINGIFY_I(x)

/**
 * Paste two tokens together without macro expansion.
 *
 * @param a First token
 * @param b Second token
 */
#define OA_CONCAT_I(a, b) a ## b

/**
 * Paste two tokens together with macro expansion.
 *
 * @param a First token
 * @param b Second token
 */
#define OA_CONCAT(a, b) OA_CONCAT_I(a, b)

/**
 * Expand the given macro arguments.
 *
 * This is useful in preprocessor macros to group comma-separated values as a
 * single function macro argument as the preprocessor will generally interpret
 * commas as argument separators (unless within parentheses).
 */
#define OA_IDENTITY(...) __VA_ARGS__

#endif  // OA_COMMON_H_
