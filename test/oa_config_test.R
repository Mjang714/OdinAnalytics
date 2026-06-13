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
  # note: SWIG adds an extra argument that we can ignore
  lib_dir <- library_dir()
  dat_dir <- data_dir()
  # check
  if (lib_dir == dat_dir) {
    return(TRUE)
  } else {
    message("library_dir() != dat_dir(): [", lib_dir, " != ", dat_dir, "]")
    return(FALSE)
  }
}

# test that VersionInfo$filename() is the file name from library_path()
test_cvi_filename <- function() {
  # path to active oa_config library
  path <- library_path()
  # oa_config filename from library_path() substring + from VersionInfo
  name_1 <- substring(path, regexpr("[/\\][^/\\]+$", path) + 1)
  name_2 <- cvi$filename()
  # check
  if (name_1 == name_2) {
    return(TRUE)
  } else {
    message(
      "substring(path, regexpr(\"[/\\][^/\\]+$\", path) + 1) != ",
      "cvi$filename(): [", name_1, " != ", name_2, "]"
    )
    return(FALSE)
  }
}

# test that VersionInfo$company() is the expected "OA Developers" string
test_cvi_company <- function() {
  s1 <- "OA Developers"
  s2 <- cvi$company()
  if (s1 == s2) {
    return(TRUE)
  } else {
    message("\"OA Developers\" != cvi$company(): [", s1, " != ", s2, "]")
    return(FALSE)
  }
}

# named vector of tests to run
tests = c(
  test_library_data_dirs = test_library_data_dirs,
  test_cvi_filename = test_cvi_filename,
  test_cvi_company = test_cvi_company
)

# main
oa.tu_exit(oa.tu_main(desc = "oa_config unit tests.", tests = tests))
