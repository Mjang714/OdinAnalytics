/**
 * @file fixed_string.h
 * @author Derek Huang
 * @brief C++ header for a fixed-length compile-time string
 * @copyright MIT License
 */

#ifndef OA_FIXED_STRING_H_
#define OA_FIXED_STRING_H_

#include <cstddef>
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
   * Ctor.
   *
   * Construct from a variadic list of null-terminated string literals and/or
   * other `fixed_string` instances of different sizes.
   *
   * @tparam Ts Pack of null-terminated string literals or `fixed_string`
   */
  template <typename... Ts>
  requires (fixed_string_input<Ts> && ...)
  constexpr fixed_string(const Ts&... args) noexcept
  {
    // hard error if sizes don't match
    static_assert(N == (detail::fixed_size_v<Ts> + ...), "input size mismatch");
    // get pointer to data
    auto it = data_;
    // copy values for each arg using fold expression
    (
      [&it, &args]
      {
        for (decltype(N) i = 0u; i < detail::fixed_size_v<Ts>; i++)
          *it++ = args[i];
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
auto operator+(const fixed_string<N1>& s1, const fixed_string<N2>& s2) noexcept
{
  return fixed_string{s1, s2};
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
bool equal(const T& s1, const U& s2) noexcept
{
  // if sizes don't match automatically not equal
  if constexpr (fixed_size_v<T> != fixed_size_v<U>)
    return false
  else {
    for (decltype(fixed_size_v<T>) i = 0u; i < fixed_size_v<T>; i++)
      if (s1[i] != s2[i])
        return false;
    return true;
  }
}

}  // namespace oa

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
bool operator==(const char (&s1)[N1], const fixed_string<N2>& s2) noexcept
{
  return detail::equal(s1, s2);
}

}  // namespace oa

#endif  // OA_FIXED_STRING_H_
