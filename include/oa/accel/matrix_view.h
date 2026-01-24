/**
 * @file matrix_view.h
 * @author Derek Huang
 * @brief C++ header providing a 2D matrix view type
 * @copyright MIT License
 */

#ifndef OA_ACCEL_MATRIX_VIEW_H_
#define OA_ACCEL_MATRIX_VIEW_H_

#include <cstddef>
#include <ranges>
#include <type_traits>

namespace oa {
namespace accel {

// forward decl for traits
template <typename T>
class matrix_view;

/**
 * Traits to check if a type is a `matrix_view<T>`.
 *
 * @tparam T type
 */
template <typename T>
struct is_matrix_view : std::false_type {};

/**
 * Partial specialization for a `matrix_view<T>`.
 *
 * @tparam T type
 */
template <typename T>
struct is_matrix_view<matrix_view<T>> : std::true_type {};

/**
 * Helper to indicate if a type is a `matrix_view<T>`.
 *
 * @tparam T type
 */
template <typename T>
constexpr bool is_matrix_view_v = is_matrix_view<T>::value;

/**
 * Row-major matrix view type.
 *
 * This provides a 2D generalization of `std::span<T>` with convenience members
 * for indexing values using `operator()`, obtaining a row slice using
 * `operator[]`, with support for per-row and flat iteration.
 *
 * For example, one can directly access elements as a 1D contiguous range:
 *
 * @code{.cc}
 * std::cout << view(i) << std::endl;
 * @endcode
 *
 * One can also access elements as a 2D row-major matrix view:
 *
 * @code{.cc}
 * std::cout << view(r, c) << std::endl;
 * @endcode
 *
 * Bracket notation is supported to get a row slice to iterate on:
 *
 * @code{.cc}
 * auto row = view[r];
 * for (auto v : row);
 * @endcode
 *
 * Additional indexing of the row slice is supported as well:
 *
 * @code{.cc}
 * std::cout << view[r][c] << std::endl;
 * @endcode
 *
 * For convenience a matrix view can be constructed from contiguous range in
 * which case the view will model a column vector:
 *
 * @code{.cc}
 * std::vector<double> vec{1., 2., 3., 4., 5.};
 * matrix_view view{vec};
 * assert(view.cols() == 1);
 * @endcode
 *
 * @tparam T type
 */
template <typename T>
class matrix_view {
public:
  using value_type = T;

  /**
   * Proxy type providing a view into a conceptual row of the data buffer.
   */
  class row_view {
  public:
    /**
     * Ctor.
     *
     * @param data Pointer to first element in row
     * @param size Number of elements in row
     */
    constexpr row_view(T* data, std::size_t size) noexcept
      : data_{data}, size_{size}
    {}

    /**
     * Return a pointer to the row data.
     */
    constexpr auto data() const noexcept { return data_; }

    /**
     * Return the size of the row.
     */
    constexpr auto size() const noexcept { return size_; }

    /**
     * Return the specified element in the row.
     *
     * @param i Row index
     */
    constexpr auto& operator[](std::size_t i) const noexcept
    {
      return data_[i];
    }

    /**
     * Return an iterator to the first element in the row.
     */
    constexpr auto begin() const noexcept
    {
      return data_;
    }

    /**
     * Return an iterator to one past the last element in the row.
     */
    constexpr auto end() const noexcept
    {
      return data_ + size_;
    }

  private:
    T* data_;
    std::size_t size_;
  };

  /**
   * Ctor.
   *
   * @param data Pointer to data buffer
   * @param row Number of rows
   * @param cols Number of columns
   */
  constexpr matrix_view(T* data, std::size_t rows, std::size_t cols = 1u) noexcept
    : data_{data}, rows_{rows}, cols_{cols}
  {}

  /**
   * Ctor.
   *
   * Create a `matrix_view` from another with a different const-qualification.
   *
   * @tparam U type
   *
   * @param other Compatible matrix view
   */
  template <typename U>
  requires (std::is_same_v<std::remove_cv_t<T>, std::remove_cv_t<U>>)
  constexpr matrix_view(const matrix_view<U>& other) noexcept
    : matrix_view{const_cast<T*>(other.data()), other.rows(), other.cols()}
  {}

  /**
   * Ctor.
   *
   * Create a `matrix_view` from a contiguous range modeling a column vector.
   *
   * @tparam R Contiguous range
   */
  template <std::ranges::contiguous_range R>
  requires (
    !is_matrix_view_v<std::decay_t<R>> &&
    std::is_same_v<std::remove_cv_t<T>, std::ranges::range_value_t<R>>
  )
  constexpr matrix_view(R&& rng) noexcept
    : matrix_view{&*std::ranges::begin(rng), std::ranges::size(rng)}
  {}

  /**
   * Return a pointer to the data buffer.
   */
  constexpr auto data() const noexcept { return data_; }

  /**
   * Return the number of rows.
   */
  constexpr auto rows() const noexcept { return rows_; }

  /**
   * Return the number of columns.
   */
  constexpr auto cols() const noexcept { return cols_; }

  /**
   * Return the total number of elements in the view.
   */
  constexpr auto size() const noexcept
  {
    return rows_ * cols_;
  }

  /**
   * Return a reference to the specified value in row-major order.
   *
   * @param i Flat index
   */
  constexpr auto& operator()(std::size_t i) const noexcept
  {
    return data_[i];
  }

  /**
   * Return a view of the specified matrix view.
   *
   * @param i Row index
   */
  constexpr auto operator[](std::size_t i) const noexcept
  {
    return row_view{data_ + i * cols_, cols_};
  }

  /**
   * Return a reference to the specified value.
   *
   * @param i Row index
   * @param j Col index
   */
  constexpr auto& operator()(std::size_t i, std::size_t j) const noexcept
  {
    return data_[i * cols_ + j];
  }

  /**
   * Return an iterator to the first element in the buffer.
   */
  constexpr auto begin() const noexcept
  {
    return data_;
  }

  /**
   * Return an iterator to one past the last element in the buffer.
   */
  constexpr auto end() const noexcept
  {
    return data_ + size();
  }

private:
  T* data_;
  std::size_t rows_;
  std::size_t cols_;
};

/**
 * User-defined deduction guide.
 *
 * @note We use `range_reference_t<R>` instead of `range_value_t<R>` to
 *  preserve const-qualification of the incoming range's value type.
 *
 * @tparam R Contiguous range
 */
template <std::ranges::contiguous_range R>
matrix_view(R&&) ->
  matrix_view<std::remove_reference_t<std::ranges::range_reference_t<R>>>;

}  // namespace accel
}  // namespace oa

#endif  // OA_ACCEL_MATRIX_VIEW_H_
