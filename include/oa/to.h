/**
 * @file to.h
 * @author Derek Huang
 * @brief C++ header for a generic type conversion template
 * @copyright MIT License
 */

#ifndef OA_TO_H_
#define OA_TO_H_

#include <type_traits>
#include <utility>

namespace oa {

/**
 * Converter class template to obtain a `T` from a `U`.
 *
 * By making the implementation a class template we allow implementation of
 * partial specializations as well as SFINAE or other template constraints/
 *
 * @tparam T Target type
 * @tparam U Source type
 */
template <typename T, typename U, typename = void>
class to_impl {};

/**
 * Convert or obtain an object of type `T` from `U` and the given arguments.
 *
 * This is a generic function that enables converting a `U` to a `T` in a
 * user-specified manner via implementation of the invocable `to_impl<T, U>`
 * class template, where any additional arguments are provided to the possibly
 * `constexpr` required implementation of `T to_impl<T, U>::operator() const`.
 * The implementation of `to_impl<T, U>::operator()` can also be `noexcept` and
 * may be overloaded or templated as appropriate.
 *
 * We provide this flexibility as one may want additional control over how `U`
 * is converted to `T` in some circumstances.
 *
 * @tparam T Target type
 * @tparam U Source type
 * @tparam Ts Additional conversion argument types
 *
 * @param v Source object
 * @param args Additional conversion arguments
 */
template <typename T, typename U, typename... Ts>
constexpr T to(U&& v, Ts&&... args)
{
  using V = std::remove_reference_t<U>;
  return to_impl<T, V>{}(std::forward<U>(v), std::forward<Ts>(args)...);
}

/**
 * Tag type for disambiguating unsafe overloads of functions.
 *
 * This is useful to disambiguate an overload that contains the "real" logic of
 * an operation to be executed after any correctness or validity checks are
 * performed on the inputs. Therefore, one can have a safe and unsafe overload,
 * where the unsafe one can be explicitly selected using `unsafe`.
 */
struct unsafe_tag {};

/**
 * Tag global for disambuigating unsafe overloads of functions.
 *
 * @todo May want to move this and `unsafe_tag` into a separate header later.
 */
inline constexpr unsafe_tag unsafe{};

}  // namespace oa

#endif  // OA_TO_H_
