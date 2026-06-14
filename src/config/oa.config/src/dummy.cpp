/**
 * @file dummy.cpp
 * @author Derek Huang
 * @brief C++ dummy for R CMD INSTALL
 * @copyright MIT License
 *
 * This file exists just to provide something for `R CMD INSTALL` to compile.
 * We instead rely on the oa_r_package() CMake function to configure `Makevars`
 * from `Makevars.in` to provide the object files and libraries to link.
 */

#ifndef __cplusplus
#error "dummy.cpp: must use C++ compiler"
#endif  // __cplusplus)
