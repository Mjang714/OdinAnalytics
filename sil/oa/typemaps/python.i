/**
 * @file typemaps/python.i
 * @author Derek Huang
 * @brief C++ SWIG Python typemaps
 * @copyright MIT License
 */

#ifndef __cplusplus
#error "typemaps/python.i: SWIG must be run in C++ mode"
#endif  // __cplusplus

#ifndef SWIGPYTHON
#error "typemaps/python.i: can only be used in SWIG Python mode"
#endif  // SWIGPYTHON

%{
#include <cstdint>
#include <filesystem>
#include <string>
#include <string_view>

#include "oa/ctti.h"
#include "oa/fixed_string.h"
#include "oa/python.h"
%}

%include "oa/common.i"  // for OA_OBJECT_TYPECHECK()

/**
 * Typemap to convert a `std::uint64_t` into a Python long.
 */
%typemap(out) std::uint64_t {
  $result = PyLong_FromUnsignedLongLong($1);
}

/**
 * Typemap to convert a `std::string` into a Python string.
 */
%typemap(out) std::string {
  $result = PyUnicode_FromStringAndSize($1.data(), $1.size());
}

/**
 * Typemap to convert a `std::string_view` into a Python string.
 *
 * There is no related string view type in Python 3, but since Python strings
 * are immutable, the semantics are not too different.
 */
%typemap(out) std::string_view {
  $result = PyUnicode_FromStringAndSize($1.data(), $1.size());
}

// helper code for the std::filesystem::path typemap conversion
%{
namespace oa {
namespace pathlib {
namespace {

/**
 * Ensure that the `pathlib` module is imported and obtain a reference.
 *
 * On error the returned Python object pointer is `nullptr`.
 *
 * @note Requires Python 3.4+.
 *
 * @par
 *
 * @note In C++20 `module` is a keyword so we avoid conflict with `module_`.
 */
auto module_() noexcept
{
  return py_object{PyImport_ImportModule("pathlib")};
}

/**
 * Get the `pathlib.Path` class object.
 *
 * On error the returned Python object pointer is `nullptr`.
 *
 * @note Requires Python 3.4+.
 */
auto Path() noexcept
{
  auto mod = module_();
  return py_object{(!mod) ? nullptr : PyObject_GetAttrString(mod, "Path")};
}

/**
 * Check if a Python object is a `pathlib.Path` instance.
 *
 * @param obj Python object
 */
bool IsPath(PyObject* obj) noexcept
{
  return !!PyObject_IsInstance(obj, Path());
}

/**
 * Convert a `pathlib.Path` or `str` into a `std::filesystem::path`.
 *
 * On error the returned `path` is empty, i.e. `empty()` returns `true`.
 *
 * @param obj Python object
 */
std::filesystem::path ToPath(PyObject* obj)
{
  // Python string object
  py_object str;
  // if pathlib.Path subclass manage its string representation
  if (IsPath(obj))
    str = py_object{PyObject_Str(obj)};
  // if a Python string itself increment the reference
  else if (PyUnicode_Check(obj))
    str = {py_object::inc, obj};
  // otherwise, set a Python exception
  else {
    fixed_string msg{
      OA_PRETTY_FUNCTION_NAME,
      ": incorrect type: input must be str or pathlib.Path"
    };
    PyErr_SetString(PyExc_TypeError, msg.data());
  }
  // handle error
  if (!str)
    return {};
  // get pointer + size of UTF-8 string
  Py_ssize_t size;
  auto utf8 = PyUnicode_AsUTF8AndSize(str, &size);
  if (!utf8)
    return {};
  // otherwise, create path
  return {utf8, utf8 + size};
}

}  // namespace
}  // namespace pathlib
}  // namespace oa
%}

/**
 * Typemap to convert a `pathlib.Path` or `str` into a `std::filesystem::path`.
 */
%typemap(in) std::filesystem::path {
  $1 = oa::pathlib::ToPath($input);
  if ($1.empty())
    SWIG_fail;
}

/**
 * Typemap to convert a `pathlib.Path` or `str` into a `std::filesystem::path`.
 *
 * This form takes a const reference so a temporary is required.
 */
%typemap(in) const std::filesystem::path& (std::filesystem::path path) {
  path = oa::pathlib::ToPath($input);
  if (path.empty())
    SWIG_fail;
  // note: reference typemaps always bind to pointers
  $1 = &path;
}

/**
 * Macro for a `std::filesystem::path` typecheck.
 *
 * This succeeds if the Python object is a `pathlib.Path` or a `str`.
 *
 * @note Typechecks are only used to disambiguate overloads.
 *
 * @param type cvref-qualified `std::filesystem::path`
 */
%define OA_FILESYSTEM_PATH_CHECK(type)
OA_OBJECT_TYPECHECK(type) {
  $1 = oa::pathlib::IsPath($input) || PyUnicode_Check($input);
}
%enddef  // OA_FILESYSTEM_PATH_CHECK(type)

OA_FILESYSTEM_PATH_CHECK(std::filesystem::path)
OA_FILESYSTEM_PATH_CHECK(const std::filesystem::path&)

/**
 * Typemap to convert a `std::filesystem::path` into a `pathlib.Path`.
 */
%typemap(out) std::filesystem::path {
  // create Python string from path (use native encoding)
  oa::py_object str{$1.c_str()};
  if (!str)
    SWIG_fail;
  // import pathlib.Path
  auto path_class = oa::pathlib::Path();
  if (!path_class)
    SWIG_fail;
  // invoke string as argument to pathlib.Path
%#if OA_PY_VERSION_IS(>=, 3, 9)
  $result = PyObject_CallOneArg(path_class, str);
%#else
  // without PyObject_CallOneArg() we have to use Py_BuildValue()
  oa::py_object tup{Py_BuildValue("(O)", str.get())};
  if (!tup)
    SWIG_fail;
  $result = PyObject_CallObject(path_class, tup);
%#endif  // !OA_PY_VERSION_IS(>=, 3, 9)
}
