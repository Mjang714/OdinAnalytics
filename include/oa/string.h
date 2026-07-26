/**
 * @file string.h
 * @author Derek Huang
 * @brief C++ header for string helpers
 * @copyright MIT License
 */

#ifndef OA_STRING_H_
#define OA_STRING_H_

#include <concepts>
#include <iterator>
#include <string_view>

#include "oa/to.h"

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
 * @param c ASCII character to check
 */
constexpr bool is_digit(char c) noexcept
{
  return c >= '0' && c <= '9';
}

/**
 * Check if the string view's characters are all ASCII digits or not.
 *
 * If the string view is empty `false` is returned.
 *
 * @param s String view to check
 */
constexpr bool is_digit(std::string_view s) noexcept
{
  // false if empty
  if (s.empty())
    return false;
  // iteratively check
  for (auto c : s)
    if (!is_digit(c))
      return false;
  return true;
}

/**
 * Character iterator to ASCII digits to integral type converter.
 *
 * Iterators are really only useful in pairs so no direct conversion is given.
 *
 * @tparam T Integral type
 */
template <std::integral T, std::forward_iterator I>
class to_impl<T, I> {
public:
  /**
   * Convert a range of ASCII digits without any checks.
   *
   * If any of the characters is not an ASCII digit or if the final value would
   * overflow the target integral type behavior is undefined. The characters in
   * the range should have already been checked using `is_digit()`.
   *
   * @note By construction this function produces only nonnegative values.
   *
   * @param it Iterator to first ASCII digit
   * @param end Iterator to one past the last ASCII digit
   */
  constexpr T operator()(I it, I end, unsafe_tag) const noexcept
  {
    // chomp any leading zeros
    while (it != end && *it == '0')
      it++;
    // build value
    T v{};
    while (it != end) {
      v = (v << 3) + v + v;            // x10 (no-op for first iteration)
      v += static_cast<T>(*it - '0');  // add converted character
      it++;                            // advance
    }
    // done
    return v;
  }
};

/**
 * String view of ASCII digits to integral type converter.
 *
 * How exactly this is done depends on the `operator()` implementation.
 *
 * @tparam T Integral type
 */
template <std::integral T>
class to_impl<T, std::string_view> {
public:
  /**
   * Convert a string view of ASCII digits without any checks.
   *
   * If any of the characters is not an ASCII digit or if the final value would
   * overflow the target integral type behavior is undefined. The string view
   * should have already been checked using `is_digit()`.
   *
   * @note By construction this function produces only nonnegative values.
   *
   * @param s String view to convert
   */
  constexpr T operator()(std::string_view s, unsafe_tag) const noexcept
  {
    return to<T>(s.begin(), s.end(), unsafe);
  }
};

}  // namespace oa

#endif  // OA_STRING_H_
