/**
 * @file common.i
 * @author Derek Huang
 * @brief C++ SWIG common multi-language helpers
 */

#ifndef __cplusplus
#error "common.i: SWIG must be run in C++ mode"
#endif  // __cplusplus

%{
#include <sstream>
#include <stdexcept>
%}

/**
 * `OA_HANDLE_EXCEPTIONS` macro for ensuring C++ exceptions are handled.
 *
 * This is necessarily defined on a per-language basis.
 */
#if defined(SWIGPYTHON)
/**
 * `OA_HANDLE_EXCEPTIONS` implementation that sets a Python `RuntimeError`.
 */
%define OA_HANDLE_EXCEPTIONS
%exception {
  try {
    $action
  }
  catch (const std::exception& exc) {
    // note: technically not noexcept
    std::stringstream ss;
    ss << "$fulldecl: " << exc.what();
    // note: temporary materialization rules mean str() is valid until the end
    // of the full-expression, i.e. destroyed only after PyErr_SetString() call
    PyErr_SetString(PyExc_RuntimeError, ss.str().c_str());
    SWIG_fail;
  }
  catch (...) {
    // note: truly exception-free call
    PyErr_SetString(PyExc_RuntimeError, "$fulldecl: unknown C++ exception");
    SWIG_fail;
  }
}
%enddef  // OA_HANDLE_EXCEPTIONS
#else
/**
 * `OA_HANDLE_EXCEPTIONS` empty stub.
 *
 * This is provided so that SWIG will not error out generically complaining
 * about some syntax error in input(1). To provide some information to the
 * consumer, however, we emit a warning, which is more helpful.
 */
%define OA_HANDLE_EXCEPTIONS
#warning "common.i: no OA_HANDLE_EXCEPTIONS definition for the target language"
%enddef  // OA_HANDLE_EXCEPTIONS
#endif  // !defined(SWIGPYTHON)

/**
 * Macro for a SWIG typecheck with `SWIG_TYPECHECK_SWIGOBJECT` precedence.
 *
 * @param type C++ type for typemap
 */
%define OA_OBJECT_TYPECHECK(type)
%typemap(typecheck, precedence=SWIG_TYPECHECK_SWIGOBJECT) type
%enddef  // OA_OBJECT_TYPECHECK(type)
