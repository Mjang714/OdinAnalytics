#!/usr/bin/bash
#
# CTest wrapper script for OdinAnalytics.
#
# Author: Derek Huang
# Copyright: MIT License
#

# program name ($0 refers to current function)
PROGNAME=$0
# current action to take, argument parsing mode
TEST_ACTION=
PARSE_ACTION=
# CTest arguments
# note: by default we are already using -j$(nproc)
CTEST_ARGS="-j$(nproc)"
# default build directory
BUILD_DIR=build

##
# Print testing wrapper usage.
#
print_usage() {
    echo "Usage: $PROGNAME" \
        "[-h] [-b BUILD_DIR] [-j[ ]PROCS] [-p] [-Ct CTEST_ARGS]"
    echo
    echo "Test driver script for OdinaAnalytics *nix builds."
    echo
    echo "Only supports single-configuration CMake generators, e.g. Makefile"
    echo "generators or Ninja, with \"Unix Makefiles\" as the default."
    echo
    echo "CTest will run \$(nproc) tests in parallel by default unless the "
    echo "-j, --parallel argument is provided. To print short progress output "
    echo "the -p, --progress option can be specified."
    echo
    echo "Options:"
    echo "  -h,  --help                     Print this usage"
    echo "  -b,  --build-dir BUILD_DIR      Build directory, default $BUILD_DIR"
    echo "  -j[ ]PROCS, --parallel PROCS    Test parallelism, default $(nproc)"
    echo "  -p, --progress                  Print short progress output"
    echo "  -Ct, --ctest-args CTEST_ARGS    Additional CTest arguments"
}

##
# Parse incoming arguments and populate CTest arguments.
#
# Arguments:
#   List of command-line arguments
#
parse_args() {
    for ARG in $@
    do
        case $ARG in
        # -h, --help
        -h | --help)
            TEST_ACTION=print_usage
            return 0
            ;;
        # -b, --build-dir
        -b | --build-dir)
            PARSE_ACTION=build_dir
            ;;
        # -j, --parallel
        -j | --parallel)
            PARSE_ACTION=parallel
            ;;
        # -j[0-9]+
        -j*)
            CTEST_ARGS="$CTEST_ARGS $ARG"
            ;;
        # -p, --progress
        -p | --progress)
            CTEST_ARGS="$CTEST_ARGS --progress"
            ;;
        # -Ct, --ctest-args
        -Ct | --ctest-args)
            PARSE_ACTION=ctest_args
            ;;
        # operate according to PARSE_ACTION
        *)
            # build directory to test
            case $PARSE_ACTION in
            build_dir)
                BUILD_DIR=$ARG
                ;;
            parallel)
                CTEST_ARGS="$CTEST_ARGS -j$ARG"
                ;;
            ctest_args)
                CTEST_ARGS="$CTEST_ARGS $ARG"
                ;;
            # no parse action
            *)
                echo "Error: Unkown option '$ARG'." \
                    "Try $PROGNAME --help for usage."
                return 1
            esac
            ;;
        esac
    done
}

##
# Main function for the script.
#
# Arguments:
#   List of command-line arguments
#
main() {
    # parse arguments + exit on error
    parse_args "$@"
    if [ $? -ne 0 ]
    then
        return $?
    fi
    # print usage or run tests
    if [ "$TEST_ACTION" == "print_usage" ]
    then
        print_usage
    else
        ctest --test-dir "$BUILD_DIR" $CTEST_ARGS
    fi
    # use last command exit status as return
    return $?
}

main "$@"
