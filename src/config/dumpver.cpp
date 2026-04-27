/**
 * @file dumpver.cpp
 * @author Derek Huang
 * @brief C++ program to display version info embedded in OA binaries
 * @copyright MIT License
 */

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iomanip>
#include <ios>
#include <iostream>
#include <string_view>

#include "oa/config/version_info.h"
#include "oa/version.h"

namespace {

// program name and usage
const auto progname = "oa-" + std::filesystem::path{__FILE__}.stem().string();
const auto program_usage = "Usage: " + progname + " [-h] [-v] FILE\n"
  "\n"
  "Print the version information embedded in a OA binary.\n"
  "\n"
  "This prints the following version information from an OA binary:\n"
  "\n"
  "  * product\n"
  "  * company\n"
  "  * description\n"
  "  * version number\n"
  "  * version string\n"
  "\n"
  "On Windows input files should be OA PE32[+] binaries while on Linux or\n"
  "related platforms input files should be ELF shared objects or executables.\n"
  "\n"
  "Optional arguments:\n"
  "  -h, --help       Print this usage\n"
  "  -v, --version    Print the OA version\n"
  "\n"
  "Required arguments:\n"
  "  FILE             OA binary file with version info to display";

/**
 * Command-line options structure.
 *
 * @param help Print the program usage
 * @param version Print the program version
 * @param path Path to binary file
 */
struct cli_options {
  bool help = false;
  bool version = false;
  std::filesystem::path path;
};

/**
 * Parse incoming command-line options.
 *
 * @param opts Command-line options to fill
 * @param argc Argument count from `main()`
 * @param argc Argument vector from `main()`
 * @returns `true` on success, `false` on error
 */
bool parse_args(cli_options& opts, int argc, char** argv)
{
  for (int i = 1; i < argc; i++) {
    // convenience argument view
    std::string_view arg{argv[i]};
    // -h, --help
    if (arg == "-h" || arg == "--help") {
      opts.help = true;
      return true;
    }
    // -v, --version
    else if (arg == "-v" || arg == "--version")
      opts.version = true;
    // unknown option
    else if (arg.size() && arg[0] == '-') {
      std::cerr << "Error: Unknown option " << arg << std::endl;
      return false;
    }
    // not an option so assign path if not yet asigned
    else if (opts.path.empty())
      opts.path = arg;
    // otherwise too many paths
    else {
      std::cerr << "Error: Too many values provided for FILE argument" <<
        std::endl;
      return false;
    }
  }
  return true;
}

}  // namespace

int main(int argc, char** argv)
{
  // parse command-line options
  cli_options opts;
  if (!parse_args(opts, argc, argv))
    return EXIT_FAILURE;
  // print usage
  if (opts.help) {
    std::cout << program_usage << std::endl;
    return EXIT_SUCCESS;
  }
  // print version and exit
  if (opts.version) {
    std::cout << OA_VERSION << std::endl;
    return EXIT_SUCCESS;
  }
  // load version info from file
  oa::config::version_info info;
  try {
    info = std::filesystem::path{argv[1]};
  }
  catch (const std::exception& exc) {
    std::cerr << "Exception: " << exc.what() << std::endl;
    return EXIT_FAILURE;
  }
  // print relevant version info
  std::cout <<
    "product:         " << info.product() << "\n" <<
    "company:         " << info.company() << "\n" <<
    "description:     " << info.description() << "\n" <<
    "version number:  0x" << info.version_hex() << "\n" <<
    "version string:  " << info.version() << "\n" <<
    std::flush;
  return EXIT_SUCCESS;
}
