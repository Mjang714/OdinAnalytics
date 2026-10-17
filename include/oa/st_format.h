/**
 * @file st_format.h
 * @author Derek Huang
 * @brief C++ header for `series` and `table` display formatting options
 * @copyright MIT License
 */

#ifndef OA_ST_FORMAT_H_
#define OA_ST_FORMAT_H_

#include <algorithm>
#include <cstddef>
#include <optional>
#include <ostream>
#include <utility>

namespace oa {

/**
 * `series` and `table` display formatting options.
 *
 * This encompasses the `series` and `table` display formatting options and
 * provides a fluent interface for retrieving and setting options.
 */
class st_format_options {
public:
  /**
   * Default ctor.
   */
  constexpr st_format_options() noexcept = default;

  /**
   * Set the max number of rows to print.
   */
  constexpr auto& max_rows(std::optional<std::size_t> v) noexcept
  {
    max_rows_ = std::move(v);
    return *this;
  }

  /**
   * Get the max number of rows to print.
   *
   * If unset this indicates there is no limit to the number of printed rows.
   */
  constexpr auto& max_rows() const noexcept
  {
    return max_rows_;
  }

  /**
   * Set the max number of columns to print.
   */
  constexpr auto& max_cols(std::optional<std::size_t> v) noexcept
  {
    max_cols_ = std::move(v);
    return *this;
  }

  /**
   * Get the max number of columns to print.
   *
   * If unset this indicates a truncated view of the columns should be printed.
   */
  constexpr auto& max_cols() const noexcept
  {
    return max_cols_;
  }

  /**
   * Indicate if truncation is needed given the number of rows to print.
   *
   * Returns `true` if `n` exceeds the greater of 10 or `max_rows()`, i.e.
   * there are excess rows beyond the display limits.
   *
   * @param n Number of rows to print
   */
  constexpr bool excess_rows(std::size_t n) const noexcept
  {
    // no limit, so no truncation
    if (!max_rows())
      return false;
    // otherwise, return result of comparison
    return n > (std::max)(std::size_t{10u}, *max_rows());
  }

  // TODO: add excess_cols()

  /**
   * Compare against another formatting options instance for equality.
   */
  constexpr bool operator==(const st_format_options&) const = default;

private:
  std::optional<std::size_t> max_rows_{50u};
  std::optional<std::size_t> max_cols_;
};

/**
 * Stream wrapper class for `series` and `table` formatting given options.
 */
class st_formatter {
public:
  /**
   * Ctor.
   *
   * @param out Output stream
   */
  st_formatter(std::ostream& out, const st_format_options& opts) noexcept
    : out_{&out}, opts_{&opts}
  {}

  /**
   * Return the output stream.
   */
  auto& stream() const noexcept
  {
    return *out_;
  }

  /**
   * Return the formatting options.
   */
  auto& options() const noexcept
  {
    return *opts_;
  }

private:
  std::ostream* out_;
  const st_format_options* opts_;
};

/**
 * Return a `st_formatter` given the table formatting options.
 *
 * The `series` and `table` implement their own `operator<<` that takes the
 * `st_formatter` as the first argument to support formatting.
 *
 * @param out Output stream
 * @param opts Table formatting options
 */
inline auto operator<<(std::ostream& out, const st_format_options& opts)
{
  return st_formatter{out, opts};
}

/**
 * Class representing a given amount of whitespace padding.
 *
 * @todo Could be moved to a general stream formatting header.
 */
class padding {
public:
  /**
   * Ctor.
   *
   * @param n Number of padding characters
   */
  constexpr padding(std::size_t n) noexcept : n_{n} {}

  /**
   * Return the padding width.
   */
  constexpr auto size() const noexcept { return n_; }

private:
  std::size_t n_;
};

/**
 * Write the specified amount of whitespace padding to the output stream.
 *
 * @param out Output stream
 * @param pad Padding specification
 */
inline auto& operator<<(std::ostream& out, const padding& pad)
{
  for (std::size_t i = 0u; i < pad.size(); i++)
    out.put(' ');
  return out;
}

}  // namespace oa

#endif  // OA_ST_FORMAT_H_
