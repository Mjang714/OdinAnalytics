/**
 * @file testing/gtest.h
 * @author Derek Huang
 * @brief Google Test compatibility header
 * @copyright MIT License
 *
 * Intended for compatibility with older Google Test versions. Defines some of
 * its own symbols as well for compatibility reasons.
 */

#ifndef OA_TESTING_GTEST_H_
#define OA_TESTING_GTEST_H_

#include <gtest/gtest.h>

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

#endif  // OA_TESTING_GTEST_H_
