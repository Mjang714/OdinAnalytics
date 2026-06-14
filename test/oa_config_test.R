# oa_config_test.R
#
# oa.config package test.
#
# Author: Derek Huang
# Copyright: MIT License
#

library(oa.config)
library(oa.testutils)

# shared VersionInfo object for the config library
cvi <- VersionInfo(library_path())

# test that library_dir() and data_dir() are the same
test_library_data_dirs <- function() {
  oa.tu_expect_eq(library_dir(), data_dir())
}

# test that VersionInfo$company() is the expected "OA Developers" string
test_cvi_company <- function() {
  oa.tu_expect_eq("OA Developers", cvi$company())
}

# test that VersionInfo$filename() is the file name from library_path()
test_cvi_filename <- function() {
  # path to active oa_config library
  path <- library_path()
  # oa_config filename from library_path() substring + from VersionInfo
  oa.tu_expect_eq(
    substring(path, regexpr("[/\\][^/\\]+$", path) + 1),
    cvi$filename()
  )
}

# test that VersionInfo$product() is the expected "OA Software Suite" string
test_cvi_product <- function() {
  oa.tu_expect_eq("OA Software Suite", cvi$product())
}

# test that VersionInfo$version_num() components are as expected
test_cvi_version_num <- function() {
  # SWIG wrapper produced an S4 class type since R only has 32-bit signed int
  vn <- cvi$version_num()
  # check each component is nonnegative
  oa.tu_expect_ge(vn$w1(), 0) &&
  oa.tu_expect_ge(vn$w2(), 0) &&
  oa.tu_expect_ge(vn$w3(), 0) &&
  oa.tu_expect_ge(vn$w4(), 0)
}

# named vector of tests to run
tests = c(
  test_library_data_dirs = test_library_data_dirs,
  test_cvi_filename = test_cvi_filename,
  test_cvi_company = test_cvi_company,
  test_cvi_product = test_cvi_product,
  test_cvi_version_num = test_cvi_version_num
)

# main
oa.tu_exit(oa.tu_main(desc = "oa_config unit tests.", tests = tests))
