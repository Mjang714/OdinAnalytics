/**
 * @file string.h
 * @author Derek Huang
 * @brief C++ header for string helpers
 * @copyright MIT License
 */

#ifndef OA_STRING_H_
#define OA_STRING_H_

namespace oa {

/**
 * Convert a character value to its ASCII uppercase equivalent.
 *
 * If the character value is not one of the ASCII lowercase values the input
 * character is returned as-is independent of character encoding.
 *
 * @param c ASCII character to convert
 */
constexpr char to_upper(char c) noexcept
{
  return (c >= 'a' && c <= 'z') ? c - 'a' + 'A' : c;
}

/**
 * Convert a character value to its ASCII lowercase equivalent.
 *
 * If the character value is not one of the ASCII uppercase values that input
 * character is returned as-is independent of character encoding.
 *
 * @param c ASCII character to convert
 */
constexpr char to_lower(char c) noexcept
{
  return (c >= 'A' && c <= 'Z') ? c - 'A' + 'a' : c;
}

/**
 * Check if a character value is an ASCII digit or not.
 *
 * @param c ASCII character to convert
 */
constexpr bool is_digit(char c) noexcept
{
  return c >= '0' && c <= '9';
}

}  // namespace oa

#endif  // OA_STRING_H_
