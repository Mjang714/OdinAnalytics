/**
 * @file oa_print_dirs.cpp
 * @author Derek Huang
 * @brief C++ program printing some OA directories
 * @copyright MIT License
 */

#include <cstdlib>
#include <iostream>

#include <oa/config/paths.h>
#include <oa/version.h>

int main()
{
  std::cout <<
    "OA version: " << OA_VERSION << "\n" <<
    "OA libdir: " << oa::config::library_dir() << "\n" <<
    "OA datadir: " << oa::config::data_dir() << "\n" << std::flush;
  return EXIT_SUCCESS;
}
