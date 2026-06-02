#ifndef OA_HELPERS_STR_UTILS_STR_UTILS_H_
#define OA_HELPERS_STR_UTILS_STR_UTILS_H_

#include <string>
#include <vector>

#include "oa/dllexport.h"

// TODO: too many namespaces that don't match directory structure
// TODO: consider subsuming into oa/string.h later
namespace oa::utils::str_utils {

/**
 * Indicate if the string is only composed of ASCII digits.
 *
 * @param str Input string
 */
OA_HELPERS_API
bool IsDigit(const std::string& str) noexcept;

/**
 * Returns a vector of integers corresponding to the digits in the string.
 *
 * If any character is not an ASCII digit an exception is thrown.
 *
 * @todo Change output type to `unsigned` or `unsigned short`.
 *
 * @param str Input string
 */
OA_HELPERS_API
std::vector<int> StrToDigits(const std::string& str);

}  // namespace oa::utils::str_utils

#endif  // OA_HELPERS_STR_UTILS_STR_UTILS_H_
