/**
 * @file helpers/warnings.h
 * @author Derek Huang
 * @brief Warning helpers
 */

#ifndef OA_HELPERS_WARNINGS_H_
#define OA_HELPERS_WARNINGS_H_

// helper macros fro disabling and re-enabling MSVC warnings
#ifdef _MSC_VER
/**
 * Push warning state.
 */
#define OA_MSVC_WARNING_PUSH() __pragma(warning(push))

/**
 * Disable specified MSVC warnings.
 *
 * @param wnos Warning number(s), e.g. 4242
 */
#define OA_MSVC_WARNING_DISABLE(wnos)  __pragma(warning(disable: wnos))

/**
 * Pop warning state.
 */
#define OA_MSVC_WARNING_POP() __pragma(warning(pop))
#else
#define OA_MSVC_WARNING_PUSH()
#define OA_MSVC_WARNING_DISABLE(wnos)
#define OA_MSVC_WARNING_POP()
#endif  // _MSC_VER

#endif  // OA_HELPERS_WARNINGS_H_
