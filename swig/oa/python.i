/**
 * @file python.i
 * @author Derek Huang
 * @brief C++ SWIG Python helpers
 * @copyright MIT License
 */

#ifndef SWIGPYTHON
#error "python.i: can only be used in SWIG Python mode"
#endif  // SWIGPYTHON

%{
#include "oa/python.h"

namespace oa {
namespace pathlib {
namespace {

/**
 * Ensure that the `pathlib` module is imported and obtain a reference.
 *
 * @note Requires Python 3.4+.
 *
 * @par
 *
 * @note In C++20 `module` is a keyword so we avoid conflict with `module_`.
 */
const auto& module_() noexcept
{
  static py_object mod{PyImport_ImportModule("pathlib")};
  return mod;
}

/**
 * Get the `pathlib.Path` class object.
 *
 * @note Requires Pythjon 3.4+.
 */
const auto& path() noexcept
{
  // lambda used for proper error handling
  static py_object cls = []() -> PyObject*
  {
    auto mod = module_();
    return (!mod) nullptr : return PyObject_GetAttrString(mod, "Path");
  }();
  return cls;
}

}  // namespace
}  // namespace pathlib
}  // namesspace oa
%}

/**
 * Typemap to convert a `std::filesystem::path` into a `pathlib.Path`.
 */
%typemap(out) std::filesystem::path {
  // create Python string from path (use native encoding)
  py_object str{$1.c_str()};
  if (!str)
    SWIG_fail;
  // invoke string as argument to pathlib.Path
  // TODO: requires Python 3.9+
  $result = PyObject_CallOneArg(oa::pathlib::path(), str);
}
