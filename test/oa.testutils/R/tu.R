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
# Return an SGR color code.
#
# Arguments:
#   val         SGR color code value, e.g. 33 for yellow, 36 for cyan
#   bright      TRUE to add bright modifier to color
#
oa.fg_color <- function(val, bright = FALSE) {
  c <- paste0("\x1b[", val, "m")
  if (bright)
    c <- paste0(c, "\x1b[1m")
  return(c)
}

# standard SGR colors
oa.fg_reset <- oa.fg_color(0)
oa.fg_red <- oa.fg_color(31)
oa.fg_green <- oa.fg_color(32)
oa.fg_yellow <- oa.fg_color(33)

# bright SGR colors
oa.fg_bright_red <- oa.fg_color(31, TRUE)
oa.fg_bright_green <- oa.fg_color(32, TRUE)
oa.fg_bright_yellow <- oa.fg_color(33, TRUE)

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
  message(oa.fg_green, "[ RUN      ] ", oa.fg_reset, name)
  # wrap in try-catch
  res <- tryCatch(
    test(),
    error = function(err) {
      message(oa.fg_red, "Exception: ", err, oa.fg_reset)
      return(FALSE)
    }
  )
  if (res) {
    message(oa.fg_green, "[     PASS ] ", oa.fg_reset, name)
    return(0)
  } else {
    message(oa.fg_red, "[     FAIL ] ", oa.fg_reset, name)
    return(1)
  }
}

##
# Test two objects for equality.
#
# On success TRUE is returned while on error a message is printed and FALSE
# returned. The expression itself is included in the error message.
#
# Arguments:
#   expected    Expected value
#   actual      Actual value
#
# Returns:
#   TRUE on equality, FALSE on error
#
oa.tu_expect_eq <- function(expected, actual) {
  if (expected == actual) {
    return(TRUE)
  } else {
    message(
      deparse(sys.call()), ": Failure\n",
      "  expected:\n",
      "    ", expected, "\n",
      "  actual:\n",
      "    ", actual
    )
    return(FALSE)
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
  message("Running ", n_tests, " test cases...")
  # no tests to run
  if (n_tests == 0) {
    message("No tests to run!")
    return(0)
  }
  # count failed
  failed <- 0
  for (i in 1:n_tests)
    failed <- failed + oa.tu_run(test_names[i], tests[[i]])
  # percentage pass rounded
  pass_pct <- round(100 * (1 - failed / n_tests))
  # if no failures, print % test passed in green
  if(!failed) {
    pct_color <- oa.fg_green
  } else {
    pct_color <- oa.fg_reset
  }
  # if failures, print number of failed in fre
  if(failed) {
    fail_color <- oa.fg_red
  } else {
    fail_color <- oa.fg_reset
  }
  # report pass/fail + return number of failed
  message(
    "\n",
    pct_color, pass_pct, "% tests passed", oa.fg_reset, ", ",
    fail_color, failed, " tests failed ", oa.fg_reset, "out of ", n_tests
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
  # options description without trailing newline
  opts_help <- r"(Options:
  -h, --help                    Print this usage
  -l, --list-tests              List the names of all available tests
  -t, --test-case <name>        Name of test case to run)"
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
      message(usage_line, "\n\n", desc, "\n\n", opts_help)
      return(0)
    # -l, --list-tests
    } else if (argv[i] == "-l" || argv[i] == "--list-tests") {
      # print the test names + exit
      for (i in 1:length(test_names))
        message(test_names[i])
      return(0)
    # -t, --test-case
    } else if (argv[i] == "-t" || argv[i] == "--test-case") {
      # need to have another argument available
      if (i == argc)
        stop("Error: -t, --test-case missing required test name")
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
      message("Warning: Unknown option \"", argv[i], "\"")
    }
  }
  # default action is to run all test cases
  return(ifelse(oa.tu_run_n(tests), 1, 0))
}
