/**
 * @file testing/gtest.h
 * @author Derek Huang
 * @brief C++ Google Test compatibility + extension header
 * @copyright MIT License
 *
 * Intended for compatibility with older Google Test versions. Defines some of
 * its own symbols as well for compatibility reasons and provides extensions.
 */

#ifndef OA_TESTING_GTEST_H_
#define OA_TESTING_GTEST_H_

#include <cstdlib>  // for OA_GTEST_ENSURE_BASE_DIR
#include <iomanip>
#include <ostream>

#include <gtest/gtest.h>

// TODO: refactor include path when namespacing is implemented
#include "time/date.h"

// GTEST_SKIP compatibility macro for pre-1.10 Google Test versions
#ifdef GTEST_SKIP
#define OA_GTEST_SKIP() GTEST_SKIP()
#else
#define OA_GTEST_SKIP() GTEST_SUCCESS_("Skipped")
#endif  // GTEST_SKIP

// macros defined for compatibility with older versions of Google Test, which
// often used "case" instead of the more modern "suite" naming convention.

#ifndef INSTANTIATE_TEST_SUITE_P
#define INSTANTIATE_TEST_SUITE_P INSTANTIATE_TEST_CASE_P
#endif  // INSTANTIATE_TEST_SUITE_P

#ifndef TYPED_TEST_SUITE
#define TYPED_TEST_SUITE TYPED_TEST_CASE
#endif  // TYPED_TEST_SUITE

#ifndef TYPED_TEST_SUITE_P
#define TYPED_TEST_SUITE_P TYPED_TEST_CASE_P
#endif  // TYPED_TEST_SUITE_P

#ifndef REGISTER_TYPED_TEST_SUITE_P
#define REGISTER_TYPED_TEST_SUITE_P REGISTER_TYPED_TEST_CASE_P
#endif  // REGISTER_TYPED_TEST_SUITE_P

#ifndef INSTANTIATE_TYPED_TEST_SUITE_P
#define INSTANTIATE_TYPED_TEST_SUITE_P INSTANTIATE_TYPED_TEST_CASE_P
#endif  // INSTANTIATE_TYPED_TEST_SUITE_P

namespace oa {
namespace time {

/**
 * Custom Google Test printer for `oa::time::Date`.
 *
 * The print format is `YYYY/MM/DD ([0-9]+)`, where the integer portion in
 * parentheses is the Julian date integer value of the date object.
 *
 * @param date Date to print
 * @param stream Stream to write to
 */
inline void PrintTo(const Date& date, std::ostream* stream)
{
  // save current stream fill character and set to '0'
  auto fill_char = stream->fill();
  stream->fill('0');
  // print in YYYY/MM/DD ([0-9]+), note std::setw resets after each expression
  *stream << std::setw(4) << date.m_years() << "/" <<
    std::setw(2) << date.m_months() << "/" <<
    std::setw(2) << date.m_days() << " (" << date.GetJulian() << ")";
  // fill character needs to be manually reset
  stream->fill(fill_char);
}

}  // namespace time
}  // namespace oa

/**
 * Macro for skipping a Google Test test if `OdinBaseDir` is not set.
 *
 * Several of the date-related tests require `OdinBaseDir` to be set in the
 * environment and could be skipped if the variable is not defined. Like
 * `GTEST_SKIP` itself this macro can only be used in `SetUp()` members or in
 * the actual Google Test test cases themselves.
 */
#define OA_GTEST_ENSURE_BASE_DIR() \
  if (!std::getenv("OdinBaseDir")) \
    OA_GTEST_SKIP() << "OdinBaseDir not set in environment"

#endif  // OA_TESTING_GTEST_H_
