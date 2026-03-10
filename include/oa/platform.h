/**
 * @file platform.h
 * @author Derek Huang
 * @brief C/C++ header for platform feature checking
 * @copyright MIT License
 */

#ifndef OA_PLATFORM_H_
#define OA_PLATFORM_H_

// TODO: remove this header after refactoring to use features.h
#include "oa/features.h"

// TODO: deprecate OA_HAS_CPP20_FORMAT and use OA_HAS_CXX20_FORMAT instead
#define OA_HAS_CPP20_FORMAT OA_HAS_CXX20_FORMAT

#endif  // OA_PLATFORM_H_
