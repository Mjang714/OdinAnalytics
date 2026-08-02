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
#include <optional>
#include <string>
#include <string_view>

#include "oa/ctti.h"
#include "oa/fixed_string.h"
#include "oa/python.h"
#include "oa/warnings.h"
%}

%include "oa/common.i"  // for OA_OBJECT_TYPECHECK()

/**
 * Typemap to convert a `Py_ssize_t` into a Python long.
 */
%typemap(out) Py_ssize_t {
  $result = PyLong_FromSsize_t($1);
}

/**
 * Typemap to convert a `std::size_t` into a Python long.
 */
%typemap(out) std::size_t {
  $result = PyLong_FromSize_t($1);
}

/**
 * Typemap to convert a `std::uint64_t` into a Python long.
 */
%typemap(out) std::uint64_t {
  $result = PyLong_FromUnsignedLongLong($1);
}

// std::string + std::string_view conversion helpers
%{
namespace oa {
namespace {

OA_GNU_WARNING_PUSH()
OA_GNU_WARNING_DISABLE(unused-function)
/**
 * Convert a Python `str` into a UTF-8 `std::string_view`.
 *
 * On error a Python exception is set and the returned optional is empty.
 *
 * @note The char data is Python managed and must not be freed manually.
 *
 * @param obj Python `str` object
 */
std::optional<std::string_view> to_utf8_view(PyObject* obj) noexcept
{
  // get buffer and size
  Py_ssize_t len;
  auto buf = PyUnicode_AsUTF8AndSize(obj, &len);
  if (!buf)
    return {};
  // return view of buffer
  return std::string_view{buf, static_cast<std::size_t>(len)};
}

/**
 * Convert a Python `str` into a UTF-8 `std::string`.
 *
 * On error a Python exception is set and the returned optional is empty.
 *
 * @param obj Python `str` object
 */
std::optional<std::string> to_utf8(PyObject* obj)
{
  auto view = to_utf8_view(obj);
  if (!view)
    return {};
  // note: string view ctor is explicit
  return std::string{*view};
}
OA_GNU_WARNING_POP()

}  // namespace
}  // namespace oa
%}

/**
 * Typemap to convert a Python string into a `std::string`.
 */
%typemap(in) std::string {
  auto str = oa::to_utf8($input);
  if (!str)
    SWIG_fail;
  $1 = std::move(*str);
}

/**
 * Typemap to convert a Python string into a `const std::string&`.
 *
 * This form takes a const reference so a temporary is required.
 *
 * @note `buf` is managed by Python and must not be freed manually.
 */
%typemap(in) const std::string& (std::optional<std::string> str) {
  str = oa::to_utf8($input);
  if (!str)
    SWIG_fail;
  // note: reference typemaps always bind to pointers
  $1 = &*str;
}

/**
 * Typemap to convert a `std::string` into a Python string.
 */
%typemap(out) std::string {
  $result = PyUnicode_FromStringAndSize($1.data(), $1.size());
}

/**
 * Typemap to convert a Python string into a `std::string_view`.
 *
 * @note `buf` is managed by Python and must not be freed manually.
 */
%typemap(in) std::string_view {
  auto view = oa::to_utf8_view($input);
  if (!view)
    SWIG_fail;
  $1 = *view;
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

/**
 * Macro for a `std::string` or `std::string_view` typecheck.
 *
 * This succeeds if the Python object is a `str`.
 *
 * @note Typechecks are only used to disambiguate overloads.
 *
 * @param type cvref-qualified `std::string` or `std::string_view`
 */
%define OA_STRING_CHECK(type)
OA_OBJECT_TYPECHECK(type) {
  $1 = PyUnicode_Check($input);
}
%enddef  // OA_STRING_CHECK(type)

OA_STRING_CHECK(std::string)
OA_STRING_CHECK(const std::string&)
OA_STRING_CHECK(std::string_view)

// helper code for the std::filesystem::path typemap conversion
%{
namespace oa {

namespace pathlib {
namespace {

OA_GNU_WARNING_PUSH()
OA_GNU_WARNING_DISABLE(unused-function)
/**
 * Ensure that the `pathlib` module is imported and obtain a reference.
 *
 * On error a Python exception is set and the returned object is empty.
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
 * On error a Python exception is set and the returned object is empty.
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
 * Convert a `std::filesystem::path` into a `pathlib.Path`.
 *
 * On error a Python exception is set and the returned `PyObject*` is `nullptr`.
 *
 * @param path Path object to convert
 */
py_object ToPath(const std::filesystem::path& path) noexcept
{
  // create Python string from path (use native encoding)
  py_object str{path.c_str()};
  if (!str)
    return {};
  // import pathlib.Path
  auto path_class = Path();
  if (!path_class)
    return {};
  // invoke string as argument to pathlib.Path
#if OA_PY_VERSION_IS(>=, 3, 9)
  return py_object{PyObject_CallOneArg(path_class, str)};
#else
  // without PyObject_CallOneArg() we have to use Py_BuildValue()
  py_object tup{Py_BuildValue("(O)", str.get())};
  if (!tup)
    return {};
  return py_object{PyObject_CallObject(path_class, tup)};
#endif  // !OA_PY_VERSION_IS(>=, 3, 9)
}
OA_GNU_WARNING_POP()

}  // namespace
}  // namespace pathlib

namespace {

OA_GNU_WARNING_PUSH()
OA_GNU_WARNING_DISABLE(unused-function)
/**
 * Convert a `pathlib.Path` or `str` into a `std::filesystem::path`.
 *
 * On error a Python exception is set and the returned optional is empty.
 *
 * @param obj Python object
 */
std::optional<std::filesystem::path> to_path(PyObject* obj)
{
  // Python string object
  py_object str;
  // if pathlib.Path subclass manage its string representation
  if (pathlib::IsPath(obj))
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
  // get view of UTF-8 string
  auto view = to_utf8_view(str);
  if (!view)
    return {};
  // otherwise, create path
  return *view;
}
OA_GNU_WARNING_POP()

}  // namespace

}  // namespace oa
%}

/**
 * Typemap to convert a `pathlib.Path` or `str` into a `std::filesystem::path`.
 */
%typemap(in) std::filesystem::path {
  auto path = oa::to_path($input);
  if (!path)
    SWIG_fail;
  // note: std::move(*path) results in an optional with a moved-from path
  $1 = *std::move(path);
}

/**
 * Typemap to convert a `pathlib.Path` or `str` into a `std::filesystem::path`.
 *
 * This form takes a const reference so a temporary is required.
 */
%typemap(in)
  const std::filesystem::path&
  (std::optional<std::filesystem::path> path)
{
  path = oa::to_path($input);
  if (!path)
    SWIG_fail;
  // note: reference typemaps always bind to pointers
  $1 = &*path;
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
  $result = oa::pathlib::ToPath($1).release();
}
