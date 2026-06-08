/**
 * @file typemaps.i
 * @author Derek Huang
 * @brief C++ SWIG typemaps
 */

#ifndef __cplusplus
#error "typemaps.i: SWIG must be run in C++ mode"
#endif  // __cplusplus

// %include typemaps appropriately based on the language
#if defined(SWIGPYTHON)
%include "oa/typemaps/python.i"
#elif defined(SWIGR)
%include "oa/typemaps/r.i"
#else
#warning "typemaps.i: no available typemaps for the target language"
#endif  // !defined(SWIGPYTHON) && !defined(SWIGR)
