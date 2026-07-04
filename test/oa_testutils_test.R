# oa_testutils_test.R
#
# oa.testutils package tests.
#
# Author: Derek Huang
# Copyright: MIT License
#
# Since oa.testutils is pure R we can add its own tests for CTest.
#

library(oa.testutils)

# numeric equality test
test_num_eq <- function() {
  oa.tu_expect_eq(1, 1)
}

# string equality test
test_str_eq <- function() {
  oa.tu_expect_eq("hello", "hello")
}

# numeric greater than or equal test
test_num_ge <- function() {
  oa.tu_expect_ge(2.4, 1.2)
}

# numeric greater than or equal test (equality)
test_num_ge_eq <- function() {
  oa.tu_expect_ge(12, 12)
}

# string lexicographic greater than or equal test
test_str_ge <- function() {
  oa.tu_expect_ge("zzz", "abcdefg")
}

# string lexicographic greater than or equal test (equality)
test_str_ge_eq <- function() {
  oa.tu_expect_ge("abcd", "abcd")
}

# named vector of test cases
tests = c(
  test_num_eq = test_num_eq,
  test_str_eq = test_str_eq,
  test_num_ge = test_num_ge,
  test_num_ge_eq = test_num_ge_eq,
  test_str_ge = test_str_ge,
  test_str_ge_eq = test_str_ge_eq
)

# main
oa.tu_exit(oa.tu_main(desc = "oa.testutils unit tests.", tests = tests))
