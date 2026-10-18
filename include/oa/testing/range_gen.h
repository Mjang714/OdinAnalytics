/**
 * @file testing/range_gen.h
 * @author Derek Huang
 * @brief C++ header for a range generation wrapper
 * @copyright MIT License
 */

#ifndef OA_TESTING_RANGE_GEN_H_
#define OA_TESTING_RANGE_GEN_H_

#include <concepts>
#include <cstddef>
#include <functional>
#include <type_traits>

namespace oa {
namespace testing {

/**
 * Concept for a `range_gen` compatible range generator invocable.
 *
 * The invocable can be nullary, i.e. invoked as `f()`, or unary, taking the
 * current index in the range as an argument, e.g. `f(i)`.
 *
 * @tparam F Invocable
 */
template <typename F>
concept range_gen_fn = std::invocable<F> || std::invocable<F, std::size_t>;

/**
 * `std::iota` projection invocable for the `range_gen`.
 *
 * This provides a non-anonymous invocable type for `range_gen` CTAD.
 *
 * @tparam T type
 */
template <typename T>
requires (std::convertible_to<std::size_t, T>)
class iota_proj {
public:
  /**
   * Ctor.
   *
   * @note Made `explicit` to prevent implicit conversion from `std::size_t`
   *  which causes ambiguity in `range_gen` ctor overload selection.
   *
   * @param v Starting value
   */
  explicit iota_proj(T v = {}) noexcept(std::is_nothrow_move_constructible_v<T>)
    : v_{std::move(v)}
  {}

  /**
   * Return the starting value plus the given index converted to `T`.
   *
   * @param i Index value
   */
  auto operator()(std::size_t i) const noexcept(std::is_arithmetic_v<T>)
  {
    return v_ + static_cast<T>(i);
  }

private:
  T v_;
};

/**
 * Range generator object.
 *
 * This takes the number of values to generate as well as a nullary or unary
 * invocable to generate the corresponding value at each iterator position. By
 * using a unary invocable, one can generate a value based on the zero-indexed
 * position of the iterator/value to generate, which is often useful.
 *
 * @tparam F Nullary or unary invocable value or reference type
 */
template <range_gen_fn F>
class range_gen {
private:
  /**
   * Traits type to retrieve the result type of `F`.
   *
   * @tparam T type
   */
  template <typename T, typename = void>
  struct result_type {};

  /**
   * Partial specialization for a nullary invocable.
   *
   * @tparam T Nullary invocable
   */
  template <typename T>
  struct result_type<T, std::enable_if_t<std::invocable<T>>> {
    using type = std::invoke_result_t<T>;
  };

  /**
   * Partial specialization for a unary invocable taking an index value.
   *
   * @note Technically should use `std::size_t&` as the invocation is on a
   *  non-const lvalue reference to a `std::size_t`.
   *
   * @tparam T Unary invocable
   */
  template <typename T>
  struct result_type<T, std::enable_if_t<std::invocable<T, std::size_t>>> {
    using type = std::invoke_result_t<T, std::size_t>;
  };

public:
  /**
   * Input iterator type.
   *
   * This satisfies the `std::input_iterator` and *LegacyInputIterator* named
   * requirements. Since there are no requirements on the `range_gen` invocable
   * being equality-preserving, `std::forward_iterator` is not satisfied.
   *
   * @note If the `range_gen` invocable is equality-preserving the `iterator`
   *  type could be enhanced to satisfy `std::random_access_iterator`, but this
   *  is not necessary, as value generation is typically a single-pass task.
   */
  class iterator {
  public:
    // type aliases to satisfy iterator traits
    // note: value_type could be a reference if F returns a reference
    using difference_type = std::ptrdiff_t;
    using value_type = typename result_type<F>::type;

    /**
     * Member access helper to preserve the lifetime of a generated value.
     *
     * This is used in the `operator->` overload as a reference-or-value
     * container to correctly preserve the lifetime of the generated value.
     */
    class value_helper {
    private:
      friend class iterator;

      /**
       * Ctor.
       *
       * @tparam T type
       */
      template <typename T>
      value_helper(T&& v) : v_{std::forward<T>(v)} {}

    public:
      /**
       * Return the address of the value or reference for member access.
       */
      auto operator->() const noexcept
      {
        return &v_;
      }

    private:
      value_type v_;
    };

    /**
     * Ctor.
     *
     * This initializes an `iterator` representing the first element.
     *
     * @param gen Range generator
     */
    iterator(range_gen& gen) noexcept : iterator{gen, 0u} {}

    /**
     * Ctor.
     *
     * @param gen Range generator
     * @param i Value index
     */
    iterator(range_gen& gen, std::size_t i) noexcept : gen_{&gen}, i_{i} {}

    /**
     * Pre-increment operator.
     */
    auto& operator++() noexcept
    {
      i_++;
      return *this;
    }

    /**
     * Post-increment operator.
     */
    auto operator++(int) noexcept
    {
      auto prev = *this;
      ++(*this);
      return prev;
    }

    /**
     * Return the value provided by the `range_gen` invocable.
     */
    decltype(auto) operator*() const
    {
      if constexpr (std::invocable<F>)
        return std::invoke(gen_->f_);
      else
        return std::invoke(gen_->f_, i_);
    }

    /**
     * Compare against another `iterator` for equality.
     *
     * This satisfies the *LegacyInputIterator* named requirements.
     *
     * @note In C++20 `operator!=` is rewritten from `operator==`.
     */
    bool operator==(const iterator& other) const noexcept
    {
      return (gen_ == other.gen_) && (i_ == other.i_);
    }

    /**
     * Member access operator.
     *
     * This satisfies the *LegacyInputIterator* named requirements.
     */
    auto operator->() const
    {
      return value_helper{**this};
    }

  private:
    range_gen* gen_;
    std::size_t i_;
  };

  /**
   * Ctor.
   *
   * This provides functionality similar to `std::iota` where the value type of
   * the range to initialize is deduced from `std::decay_t<T>` itself.
   *
   * @tparam T Arithmetic type or other convertible from `std::size_t`
   *
   * @param n Number of elements to generate
   * @param v Value of first element
   */
  template <typename T>
  requires (std::convertible_to<std::size_t, T>)
  range_gen(std::size_t n, T&& v) : range_gen{n, iota_proj{std::forward<T>(v)}}
  {}

  /**
   * Ctor.
   *
   * @tparam G Nullary or unary invocable
   *
   * @param n Number of elements to generate
   * @param f Nullary or unary invocable
   */
  template <range_gen_fn G>
  range_gen(std::size_t n, G&& f) : n_{n}, f_{std::forward<G>(f)} {}

  /**
   * Return the number of elements to generate.
   */
  auto n() const noexcept { return n_; }

  /**
   * Return an iterator to the first value.
   */
  iterator begin() noexcept
  {
    return *this;
  }

  /**
   * Return an iterator to one past the last value.
   */
  iterator end() noexcept
  {
    return {*this, n_};
  }

private:
  std::size_t n_;
  F f_;
};

/**
 * User-defined deduction guide for `range_gen`.
 *
 * This supports defining the `F` member as either an lvalue reference to a
 * possibly const-qualified type or simply a value type based on whether an
 * lvalue or rvalue was provided to the forwarding reference.
 *
 * @tparam F Nullary or unary invocable value or reference type
 */
template <range_gen_fn F>
range_gen(std::size_t, F&&) -> range_gen<F>;

/**
 * User-defined deduction guide for `range_gen`.
 *
 * This supports the `std::iota`-like ctor overload with `iota_proj<T>`.
 *
 * @tparam T Arithmetic type or other convertible from `std::size_t`
 */
template <typename T>
requires (std::convertible_to<std::size_t, T>)
range_gen(std::size_t, T&&) -> range_gen<iota_proj<std::decay_t<T>>>;

}  // namespace testing
}  // namespace oa

#endif  // OA_TESTING_RANGE_GEN_H_
