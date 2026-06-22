/**
 * @file string.h
 * @author Derek Huang
 * @brief C++ header for string helpers
 * @copyright MIT License
 */

#ifndef OA_STRING_H_
#define OA_STRING_H_

#include <ostream>
#include <type_traits>

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
 * Tag type to represent formatting a value as hex bytes.
 */
struct hex_tag {};

/**
 * Tag global to represent formatting a value as hex bytes.
 */
inline constexpr hex_tag hex{};

/**
 * Output stream wrapper for formatting a value as hex bytes.
 */
class hex_ostream_wrapper {
public:
  hex_ostream_wrapper(std::ostream& out) noexcept : out_{&out} {}
  auto& out() const noexcept { return *out_; }
private:
  std::ostream* out_;
};

/**
 * Formatter template to define how a type should be streamed as hex bytes.
 *
 * All valid specializations must implement the following:
 *
 * @code{.cc}
 * void operator()(std::ostream&, U) const
 * @endcode
 *
 * Here `U` is a possibly cv-qualified, ref-qualified `T`. The output should be
 * something like `"0x4A2BDF15"`, e.g. for a 4-byte integral type, or for a
 * larger structure, `"AB 3D 12 E1 A3 4D"` without any `"0x"` prefix.
 *
 * @tparam T type
 */
template <typename T, typename = void>
class hex_stream_formatter {};

/**
 * Partial specialization for integral types.
 *
 * The format will look something like `"0x4D51231D"` for example. Endianness
 * issues are correctly handled by operating byte-by-byte on values.
 *
 * @note Since function and member function pointers can have sizes that are
 *  *not* the same as `sizeof(void*)`, for "standard" pointer to objects or
 *  non-static data members, cast the pointer to `std::uintptr_t`.
 *
 * @tparam T type
 */
template <typename T>
class hex_stream_formatter<T, std::enable_if_t<std::is_integral_v<T>> > {
public:
  void operator()(std::ostream& out, T v) const
  {
    // hex characters
    constexpr auto hchars = "0123456789ABCDEF";
    // helper to extract a single byte as a char. we do this instead of
    // interpreting the bytes in order to avoid endianness issues
    auto byte = [v](auto i) noexcept
    {
      // shift size in terms of octets
      auto shift = 8u * (sizeof(v) - i - 1u);
      // get single octet by masking with 0xFF
      return static_cast<char>((v & (0xFF << shift)) >> shift);
    };
    // for each byte in write 2 hex chars
    out << "0x";
    for (auto i = 0u; i < sizeof(v); i++)
      out << hchars[(byte(i) >> 4u) & 0xF] << hchars[byte(i) & 0xF];
  }
};

/**
 * Get a new `hex_ostream_wrapper` for formatting the object as hex bytes.
 *
 * @note All parameters are taken by reference to avoid MSVC emitting C4866.
 *
 * @param out Output stream
 */
inline auto operator<<(std::ostream& out, const hex_tag&) noexcept
{
  return hex_ostream_wrapper{out};
}

/**
 * Stream a hex representation for the given object.
 *
 * @note All parameters are taken by reference to avoid MSVC emitting C4866.
 *
 * @tparam T type
 *
 * @param out Output formatter
 * @param val Object to format
 * @returns `std::ostream&`
 */
template <typename T>
auto& operator<<(const hex_ostream_wrapper& out, const T& val)
{
  hex_stream_formatter<T>{}(out.out(), val);
  return out.out();
}

}  // namespace oa

#endif  // OA_STRING_H_
