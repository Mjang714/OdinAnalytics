# tu.R
#
# Main oa.testutils helpers file.
#
# Author: Derek Huang
# Copyright: MIT License
#
# This contains the majority of the useful testing-related functions. Note that
# "tu" is an abbreviation for "testutils".
#

##
# Run a single named test.
#
# The test function must take no parameters and return TRUE on success, FALSE
# on error, and is encouraged to print meaningful test messages.
#
# Arguments:
#   name        Test name
#   test        Test function
#
# Returns:
#   0 on success, 1 on failure; interpret as number of test failures
#
oa.tu_run <- function(name, test) {
  cat("[ RUN      ]", name, fill = TRUE)
  if (test()) {
    cat("[     PASS ]", name, fill = TRUE)
    return(0)
  } else {
    cat("[     FAIL ]", name, fill = TRUE)
    return(1)
  }
}

##
# Run a list of named tests.
#
# The tests object must be a named vector where each value is a function that
# returns TRUE on success, FALSE on error, and wil be invoked by tu_run().
#
# Arguments:
#   tests         Named vector of tests
#
# Returns:
#   Number of failed tests
#
oa.tu_run_n <- function(tests) {
  # test names
  test_names <- names(tests)
  # default action is to run all test cases
  n_tests <- length(tests)
  cat("Running", n_tests, "test cases...", fill = TRUE)
  # no tests to run
  if (n_tests == 0) {
    cat("No tests to run!", fill = TRUE)
    return(0)
  }
  # count failed
  failed <- 0
  for (i in 1:n_tests)
    failed <- failed + oa.tu_run(test_names[i], tests[[i]])
  # percentage pass
  pass_pct <- 100 * (1 - failed / n_tests)
  # report pass/fail + return number of failed
  cat(
    pass_pct,
    "% tests passed, ",
    failed,
    " tests failed out of ",
    n_tests,
    sep = "",
    fill = TRUE
  )
  return(failed)
}

##
# Standard main function for an R unit test script.
#
# This function parses incoming command-line arguments to determine the mode of
# operation and will run the tests specified in the named vector of tests using
# oa.tu_run_n(). The -l, --list-tests argument causes the names of the tests to
# be printed, while -t, --test-case is used to specify a single test to run.
#
# Arguments:
#   argv          Command-line arguments to parse
#   desc          Text to display as part of usage printed for -h, --help
#   tests         Named vector of tests to run
#
# Returns:
#   Exit code, 0 for success, 1 for failure
#
oa.tu_main <- function(
  argv = commandArgs(trailingOnly = TRUE),
  desc = "",
  tests = c()
) {
  # obtain R basename + file path index
  r_path <- basename(commandArgs()[1])
  file_i <- grep("--file=", commandArgs())
  # if NA, use "<stdin>", else strip --file= prefix + convert to base name
  if (is.na(file_i)) {
    file_name <- "<stdin>"
  } else {
    file_name <- basename(substring(commandArgs()[file_i], 8))
  }
  # usage description
  usage_line <- paste("Usage:", r_path, file_name, "[-h] [-l] [-t TEST_CASE]")
  # default description if desc is empty
  if (desc == "")
    desc <- "R test runner."
  # options description with trailing newline
  opts_help <- r"(Options:
  -h, --help                    Print this usage
  -l, --list-tests              List the names of all available tests
  -t, --test-case <name>        Name of test case to run
)"
  # number of arguments
  argc <- length(argv)
  # test names
  test_names <- names(tests)
  # iterate through arguments
  for (i in 1:argc) {
    # break if no more arguments, e.g. if argv is empty
    if (is.na(argv[i]))
      break
    # -h, --help
    if (argv[i] == "-h" || argv[i] == "--help") {
      cat(usage_line, "\n\n", desc, "\n\n", opts_help, sep = "")
      return(0)
    # -l, --list-tests
    } else if (argv[i] == "-l" || argv[i] == "--list-tests") {
      # print the test names + exit
      for (i in 1:length(test_names))
        cat(test_names[i], fill = TRUE)
      return(0)
    # -t, --test-case
    } else if (argv[i] == "-t" || argv[i] == "--test-case") {
      # need to have another argument available
      if (i == argc)
        stop(paste("Error: -t, --test-case missing required test name"))
      # look up test case
      test_name <- argv[i + 1]
      test_case <- tests[[test_name]]
      if (is.null(test_case))
        stop(paste("Error: No test named", test_name, "in list of tests"))
      # ok, run test and return 0 (success) or 1 (failure)
      return(oa.tu_run(test_name, test_case))
    # ignore --no-echo, --no-restore, --file=<script path> added by Rscript
    } else if (
      argv[i] == "--no-echo" ||
      argv[i] == "--no-restore" ||
      substr(argv[i], 1, 7) == "--file="
    ) {
    # ignore unknown options in case we haven't fully covered them
    } else {
      cat("Warning: Unknown option \"", argv[i], "\"", sep = "", fill = TRUE)
    }
  }
  # default action is to run all test cases
  return(ifelse(oa.tu_run_n(tests), 1, 0))
}
