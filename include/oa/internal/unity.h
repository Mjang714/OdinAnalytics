/**
 * @file internal/unity.h
 * @author Derek Huang
 * @brief C/C++ header for unity build support
 * @copyright MIT License
 *
 * This internal header provides some support for unity builds, where multiple
 * translation units are effectively `#include`d together as a single
 * agglomeration to reduce the cost of repeated preprocessing per translation
 * unit. Two macros are provided for unity build support:
 *
 * 1. `OA_UNITY_ID`, which should be uniquely defined to a valid identifier for
 *    each unity translation unit by the build system (e.g. CMake)
 * 2. `OA_UNITY_BUILD`, which will be 1 or 0 depending on whether `OA_UNITY_ID`
 *    has been externally defined or not (although we can't validate whether or
 *    not `OA_UNITY_ID` is uniquely defined per unity TU).
 *
 * Although problematic source files can be excluded from unity builds, e.g.
 * setting the `SKIP_UNITY_BUILD_INCLUSION` property on source files in CMake,
 * one can use `OA_UNITY_ID` as a unique, per-unity-TU namespace within an
 * anonymous namespace to ensure that local symbols are unique. For example:
 *
 * @code{.cc}
 * namespace {
 * namespace OA_UNITY_ID {
 *
 * void foo();
 *
 * }  // namespace OA_UNITY_ID
 * }  // namespace
 *
 * int main()
 * {
 *   OA_UNITY_ID::foo();
 *   return EXIT_SUCCESS;
 * }
 * @endcode
 *
 * When not doing a unity build `OA_UNITY_ID` is defined to be empty. CMake
 * integration is via setting the `UNITY_BUILD_UNIQUE_ID` target property to
 * `OA_UNITY_ID`. We choose to do this using the `oa_unity_enable_unique_id()`
 * CMake function as having a real function allows us to avoid typos when
 * manually setting `UNITY_BUILD_UNIQUE_ID` for a target.
 *
 * For other build systems, they would need to automatically define
 * `OA_UNITY_ID` uniquely for every unity translation unit being compiled.
 */

#ifndef OA_INTERNAL_UNITY_H_
#define OA_INTERNAL_UNITY_H_

// if defined, should be unique per unity TU, and provided by build system
#if defined(OA_UNITY_ID)
// note: Doxygen comment attached to case where OA_UNITY_ID is not defined
#define OA_UNITY_BUILD 1
#else
/**
 * Odin unity build per-translation-unit unique identifier.
 *
 * This should be defined to a valid identifier, unique per agglomerated unity
 * translation unit, during compilation by a build system, e.g. CMake. See
 * https://cmake.org/cmake/help/latest/prop_tgt/UNITY_BUILD_UNIQUE_ID.html for
 * details on how this can be done using CMake for a single target.
 */
#define OA_UNITY_ID

/**
 * Compile-time macro indicating if a unity build is being done or not.
 *
 * This enables one to do some conditional compilation based on whether a unity
 * build is being done or not. Although discouraged, it may be useful.
 */
#define OA_UNITY_BUILD 0
#endif  // !defined(OA_UNITY_ID)

#endif  // OA_INTERNAL_UNITY_H_
