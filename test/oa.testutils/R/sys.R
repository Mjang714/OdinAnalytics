# sys.R
#
# oa.testutils system helpers.
#
# Author: Derek Huang
# Copyright: MIT License
#

##
# Exit the running R script with the given exit code.
#
# This function does not save the current R workspace.
#
# Arguments:
#   status      Exit code
#
oa.tu_exit <- function(status = 0) {
  quit("no", status)
}

##
# Load a shared object given its filename stem.
#
# The extension is automatically determined using .Platform$dynlib.ext.
#
# Arguments:
#   name        DSO file name stem
#
oa.tu_load <- function(name) {
  dyn.load(paste(name, .Platform$dynlib.ext, sep = ""))
}
