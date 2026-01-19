/**
 * @file xl_conv.h
 * @author Derek Huang
 * @brief C++ header for `XLCALL.H` type converters
 * @copyright MIT License
 */

#ifndef OA_ACCEL_XL_CONV_H_
#define OA_ACCEL_XL_CONV_H_

#include <string>
#include <string_view>

// forward decl to avoid pulling in XLCALL.H
struct xloper12;

namespace oa {
namespace accel {

/**
 * Get the `double` value stored in a `XLOPER12` of type `xltypeNum`.
 *
 * Conversions are done if the type is `xltypeBool` or `xltypeInt` and an
 * exception is otherwise thrown to indicate conversion failure.
 *
 * @param op `XLOPER12` to convert
 */
double as_double(const xloper12& op);

/**
 * Get the string stored in a `XLOPER12` of type `xltypeStr` as a string.
 *
 * Conversions are done if the type is `xltypeNum`, `xltypeBool`, `xltypeErr`,
 * and `xltypeInt`, with exception thrown otherwise.
 *
 * @note This does *not* provide the same result as `operator<<` as only a few
 *  conversions are allowed due to different intended semantics.
 *
 * @param op `XLOPER12` to convert
 */
std::string as_string(const xloper12& op);

/**
 * Get the string stored in a `XLOPER12` of type `xltypeStr` as a wide string.
 *
 * Conversions are done if the type is `xltypeNum`, `xltypeBool`, `xltypeErr`,
 * and `xltypeInt`, with exception thrown otherwise.
 *
 * @param op `XLOPER12` to convert
 */
std::wstring as_wstring(const xloper12& op);

/**
 * Get a view of the wide string stored in a `XLOPER12` of type `xltypeStr`.
 *
 * This provides a direct view into the `XLOPER12` wide character buffer and
 * therefore saves a copy if the string content is rather long. An exception is
 * thrown if the `XLOPER12` is not a `xltypeStr` as converting is not possible.
 *
 * @param op `XLOPER12` to get view of
 */
std::wstring_view as_wstring_view(const xloper12& op);

}  // namespace oa
}  // namespace oa

#endif  // OA_ACCEL_XL_CONV_H_
