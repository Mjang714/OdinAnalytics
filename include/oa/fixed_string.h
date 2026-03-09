/**
 * @file fixed_string.h
 * @author Derek Huang
 * @brief C++ header for a fixed-length compile-time string
 * @copyright MIT License
 */

#ifndef OA_FIXED_STRING_H_
#define OA_FIXED_STRING_H_

#include <concepts>
#include <cstddef>
#include <limits>
#include <ostream>
#include <string>
#include <type_traits>

namespace oa {

// forward decl for traits
template <std::size_t N>
class fixed_string;

/**
 * Traits type checking if a type is a `fixed_string<N>`.
 *
 * @tparam T type
 */
template <typename T>
struct is_fixed_string : std::false_type {};

/**
 * True specialization for a `fixed_string<N>`.
 *
 * @tparam N Number of characters in string excluding null terminator
 */
template <std::size_t N>
struct is_fixed_string<fixed_string<N>> : std::true_type {};

/**
 * Indicate if a type is a `fixed_string<N>`.
 *
 * @tparam T type
 */
template <typename T>
constexpr bool is_fixed_string_v = is_fixed_string<T>::value;

namespace detail {

/**
 * Indicate if a type is a character array.
 *
 * @tparam T type
 */
template <typename T>
struct is_char_array : std::false_type {};

/**
 * True specialization for a character array.
 *
 * @tparam N Number of characters in the string literal array
 */
template <std::size_t N>
struct is_char_array<char[N]> : std::true_type {};

/**
 * Indicate if the type is a reference to a character array.
 *
 * @tparam T type
 */
template <typename T>
constexpr bool is_char_array_v = is_char_array<T>::value;

/**
 * Traits type to obtain the compile-time size of a range-like type.
 *
 * By default the size is simply 1; e.g. non-range types constitute 1 element.
 *
 * @tparam T type
 */
template <typename T>
struct fixed_size {
  static constexpr std::size_t value = 1u;
};

/**
 * Partial specialization for character arrays.
 *
 * This assumes that the character arrays are string literals, i.e. that they
 * are null-terminated, hence why we subtract one from the actual size.
 *
 * @tparam N Number of elements in the array
 */
template <std::size_t N>
struct fixed_size<char[N]> {
  static constexpr auto value = N - 1u;
};

/**
 * Partial specialization for the `fixed_string<N>`.
 *
 * @tparam N Number of elements in the `fixed_string`
 */
template <std::size_t N>
struct fixed_size<fixed_string<N>> {
  static constexpr auto value = N;
};

/**
 * Obtain the compile-time size for the given type.
 *
 * @tparam T type
 */
template <typename T>
constexpr auto fixed_size_v = fixed_size<T>::value;

}  // namespace detail

/**
 * Concept for a `fixed_string` ctor input.
 *
 * This is currently either a `fixed_string` or a character array.
 *
 * @tparam T
 */
template <typename T>
concept fixed_string_input = is_fixed_string_v<T> || detail::is_char_array_v<T>;

/**
 * Class representing a string of fixed compile-time size.
 *
 * This is used as a way to create a null-terminated concatenated copy of
 * strings with known compile-time size, in particular string literals.
 *
 * @tparam N Number of characters in string excluding null terminator
 */
template <std::size_t N>
class fixed_string {
public:
  /**
   * Constant representing the maximum `fixed_string` length.
   */
  static constexpr auto npos = (std::numeric_limits<std::size_t>::max)();

  /**
   * Ctor.
   *
   * Construct from a variadic list of null-terminated string literals and/or
   * other `fixed_string` instances of different sizes.
   *
   * If the size of the `fixed_string` is explicitly provided instead of
   * obtained through CTAD it can be specified to be smaller than the total
   * number of the input characters provided by the inputs.
   *
   * @tparam Ts Pack of null-terminated string literals or `fixed_string`
   */
  template <typename... Ts>
  requires (fixed_string_input<Ts> && ...)
  constexpr fixed_string(const Ts&... args) noexcept
  {
    // hard error if sizes don't match
    static_assert(N <= (detail::fixed_size_v<Ts> + ...), "too few inputs");
    // get pointer to data
    auto it = data_;
    // copy values for each arg using fold expression
    (
      [this, &it, &args]
      {
        decltype(N) i = 0u;
        // note: need to stop if N is already reached
        while ((it < data_ + N) && i < detail::fixed_size_v<Ts>)
          *it++ = args[i++];
      }(), ...
    );
    // write final null terminator
    *it = '\0';
  }

  /**
   * Return pointer to the first character in the `fixed_string`.
   *
   * If the `fixed_string` has size zero this points to the `'\0'`.
   */
  constexpr auto data() const noexcept { return data_; }

  /**
   * Return the size of the string.
   */
  constexpr auto size() const noexcept { return N; }

  /**
   * Return an iterator to the first character.
   */
  constexpr auto begin() const noexcept { return data_; }

  /**
   * Return an iterator one past the last character.
   */
  constexpr auto end() const noexcept { return data_ + N; }

  /**
   * Return a const reference to the `i`th character.
   *
   * @param i Index starting from zero
   */
  constexpr auto& operator[](std::size_t i) const noexcept
  {
    return data_[i];
  }

  /**
   * Return the position of the first occurrence of the character.
   *
   * If not found then `npos` will be returned.
   *
   * @tparam Ts `char`
   *
   * @param cs Characters to find
   */
  template <std::same_as<char>... Ts>
  constexpr auto find(Ts... cs) const noexcept
  {
    // note: use separate array for char pack since MSVC seems to have an issue
    // with destroying initializer_list values too early, e.g. in this context
    // where for (auto c : {cs...}) was used as the inner loop
    const char ar[] = {cs...};
    // search
    for (auto i = 0u; i < N; i++)
      for (auto c : ar)
        if (data_[i] == c)
          return i;
    return npos;
  }

  /**
   * Return the position of the last occurrence of the character.
   *
   * If not found then `npos` will be returned.
   *
   * @tparam Ts `char`
   *
   * @param cs Characters to find
   */
  template <std::same_as<char>... Ts>
  constexpr auto rfind(Ts... cs) const noexcept
  {
    // note: use separate array for char pack since MSVC seems to have an issue
    // with destroying initializer_list values too early, e.g. in this context
    // where for (auto c : {cs...}) was used as the inner loop
    const char ar[] = {cs...};
    // search
    for (auto i = 0u; i < N; i++)
      // for (auto c : ar)
      for (auto c : ar)
        if (data_[N - i - 1u] == c)
          return N - i - 1u;
    return npos;
  }

  /**
   * Return a `fixed_string` corresponding to a substring.
   *
   * @tparam I Index of first element
   * @tparam N_ Length of substring
   */
  template <std::size_t I, std::size_t N_ = N - I>
  constexpr auto substr() const noexcept
  {
    // copy character range
    char str[N_ + 1u];
    for (decltype(N) i = 0u; i < N_; i++)
      str[i] = data_[I + i];
    // convert to fixed_string
    // note: need to specify N_ otherwise injected class name is used
    return fixed_string<N_>{str};
  }

  /**
   * Enable implicit conversion to a const-qualified null-terminated string.
   *
   * This is useful for interop with C functions or other conversions.
   */
  constexpr operator const char*() const noexcept
  {
    return data_;
  }

private:
  char data_[N + 1u];
};

// user-defined deduction guide for CTAD
template <typename... Ts>
requires (fixed_string_input<Ts> && ...)
fixed_string(const Ts&...) -> fixed_string<(detail::fixed_size_v<Ts> + ...)>;

/**
 * Concatenate two `fixed_string` instances.
 *
 * @tparam N1 Length of first fixed string
 * @tparam N2 Length of second fixed string
 *
 * @param s1 First fixed string
 * @param s2 Second fixed string
 */
template <std::size_t N1, std::size_t N2>
constexpr
auto operator+(const fixed_string<N1>& s1, const fixed_string<N2>& s2) noexcept
{
  return fixed_string{s1, s2};
}

/**
 * Concatenate a `fixed_string` with a null-terminated character array.
 *
 * @tparam N1 Length of string
 * @tparam N2 Length of character array
 *
 * @param s1 Fixed string
 * @param s2 Null-terminated char array
 */
template <std::size_t N1, std::size_t N2>
constexpr
auto operator+(const fixed_string<N1>& s1, const char (&s2)[N2]) noexcept
{
  return fixed_string{s1, s2};
}

/**
 * Concatenate a null-terminated character array with a `fixed_string`.
 *
 * @tparam N1 Length of character array
 * @tparam N2 Length of string
 *
 * @param s1 Null-terminated char array
 * @param s2 Fixed string
 */
template <std::size_t N1, std::size_t N2>
constexpr
auto operator+(const char (&s1)[N1], const fixed_string<N2>& s2) noexcept
{
  return fixed_string{s1, s2};
}

/**
 * Return a `std::string` from a `fixed_string` and a `std::string`.
 *
 * @tparam N Length of fixed string
 *
 * @param s1 First string
 * @param s1 Second string
 */
template <std::size_t N>
constexpr auto operator+(const fixed_string<N>& s1, const std::string& s2)
{
  return s1.data() + s2;
}

/**
 * Return a `std::string` from a `std::string` and a `fixed_string`.
 *
 * @tparam N Length of fixed string
 *
 * @param s1 FIrst string
 * @param s1 Second string
 */
template <std::size_t N>
constexpr auto operator+(const std::string& s1, const fixed_string<N>& s2)
{
  return s1 + s2.data();
}

namespace detail {

/**
 * Compare two `fixed_string` or null-terminated character arrays for equality.
 *
 * To avoid introducing a `operator==` for character arrays in the global
 * namespace we implement the shared functionality using this function.
 *
 * @tparam T First type
 * @tparam U Second type
 *
 * @param s1 First fixed string or character array
 * @param s2 Second fixed string or character array
 */
template <fixed_string_input T, fixed_string_input U>
constexpr bool equal(const T& s1, const U& s2) noexcept
{
  // if sizes don't match automatically not equal
  if constexpr (fixed_size_v<T> != fixed_size_v<U>)
    return false;
  else {
    for (std::size_t i = 0u; i < fixed_size_v<T>; i++)
      if (s1[i] != s2[i])
        return false;
    return true;
  }
}

}  // namespace detail

/**
 * Compare two `fixed_string` instances for equality.
 *
 * They are automatically not equal if the lengths differ.
 *
 * @tparam N1 Length of first fixed string
 * @tparam N2 Length of second fixed string
 *
 * @param s1 First fixed string
 * @param s2 Second fixed string
 */
template <std::size_t N1, std::size_t N2>
constexpr
bool operator==(const fixed_string<N1>& s1, const fixed_string<N2>& s2) noexcept
{
  return detail::equal(s1, s2);
}

/**
 * Compare a `fixed_string` against a null-terminated char array for equality.
 *
 * @tparam N1 Length of fixed string
 * @tparam N2 Length of character array
 *
 * @param s1 Fixed string
 * @param s2 Null-terminated char array
 */
template <std::size_t N1, std::size_t N2>
constexpr
bool operator==(const fixed_string<N1>& s1, const char (&s2)[N2]) noexcept
{
  return detail::equal(s1, s2);
}

/**
 * Compare a null-terminated char array against a `fixed_string` for equality.
 *
 * @tparam N1 Length of character array
 * @tparam N2 Length of fixed string
 *
 * @param s1 Null-terminated char array
 * @param s2 Fixed string
 */
template <std::size_t N1, std::size_t N2>
constexpr
bool operator==(const char (&s1)[N1], const fixed_string<N2>& s2) noexcept
{
  return detail::equal(s1, s2);
}

/**
 * Compare a `fixed_string` and a `std::string` for equality.
 *
 * @tparam N Length of fixed string
 *
 * @param s1 First string
 * @param s2 Second string
 */
template <std::size_t N>
constexpr
bool operator==(const fixed_string<N>& s1, const std::string& s2)
{
  return s1.data() == s2;
}

/**
 * Compare a `fixed_string` and a `std::string` for equality.
 *
 * @tparam N Length of fixed string
 *
 * @param s1 First string
 * @param s2 Second string
 */
template <std::size_t N>
constexpr
bool operator==(const std::string& s1, const fixed_string<N>& s2)
{
  return s2 == s1;
}

/**
 * Write the contents of a `fixed_string` to an output stream.
 *
 * @tparam N Length of fixed string
 *
 * @param out Output stream
 * @param str Fixed string to write
 */
template <std::size_t N>
auto& operator<<(std::ostream& out, const fixed_string<N>& str)
{
  return out.write(str.data(), str.size());
}

}  // namespace oa

#endif  // OA_FIXED_STRING_H_
