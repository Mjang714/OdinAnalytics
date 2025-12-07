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
 * @note Requires Python 3.4+.
 */
const auto& Path() noexcept
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
  oa::py_object str{$1.c_str()};
  if (!str)
    SWIG_fail;
  // import pathlib.Path
  const auto& path_class = oa::pathlib::Path();
  if (!path_class)
    SWIG_fail;
  // invoke string as argument to pathlib.Path
#if OA_PY_VERSION_IS(>=, 3, 9)
  $result = PyObject_CallOneArg(path_class, str);
#else
  // without PyObject_CallOneArg() we have to use Py_BuildValue()
  oa::py_object tup{Py_BuildValue("(O)", )}
  if (!tup)
    SWIG_fail;
  $result = PyObject_CallObject(path_class, tup);
#endif  // !OA_PY_VERSION_IS(>=, 3, 9)
}
