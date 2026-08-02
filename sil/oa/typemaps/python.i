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
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <utility>

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

// helper code for scoped enum and enum.Enum interop
%{
namespace oa {

namespace {

OA_GNU_WARNING_PUSH()
OA_GNU_WARNING_DISABLE(unused-function)
/**
 * Check that the Python object's `_cxx_type_name` matches the given string.
 *
 * The `_cxx_type_name` class attribute is used to hold the fully-qualified C++
 * scoped enum type name for an `enum.Enum` representing the exported enum.
 *
 * On error a Python exception is set and `false` is returned.
 *
 * @param obj Python object to check
 * @param type_name Expected fully-qualified C++ type name
 */
bool check_cxx_type_name(PyObject* obj, std::string_view type_name) noexcept
{
  // get attribute
  py_object attr{PyObject_GetAttrString(obj, "_cxx_type_name")};
  if (!attr)
    return false;
  // get view of string value
  auto view = to_utf8_view(attr);
  if (!view)
    return false;
  // if view doesn't match the expected value, error
  if (*view != type_name) {
    // note: techically not noexcept
    std::stringstream ss;
    ss << OA_PRETTY_FUNCTION_NAME << ": _cxx_type_name value " << *view <<
      " != expected C++ type name " << type_name;
    // note: std::move() to move instead of copy + temporarily materialize
    PyErr_SetString(PyExc_TypeError, std::move(ss).str().c_str());
    return false;
  }
  // ok, return true
  return true;
}
OA_GNU_WARNING_POP()

}  // namespace

namespace enum_ {
namespace {

OA_GNU_WARNING_PUSH()
OA_GNU_WARNING_DISABLE(unused-function)
/**
 * Ensure that the `enum` module is imported and obtain a reference.
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
  return py_object{PyImport_ImportModule("enum")};
}

/**
 * Get the `enum.Enum` class object.
 *
 * On error a Python exception is set and the returned object is empty.
 *
 * @note Requires Python 3.4+.
 */
auto Enum() noexcept
{
  auto mod = module_();
  return py_object{(!mod) ? nullptr : PyObject_GetAttrString(mod, "Enum")};
}

/**
 * Check if a Python object is an `enum.Enum` instance.
 *
 * @param obj Python object
 */
bool IsEnum(PyObject* obj) noexcept
{
  return !!PyObject_IsInstance(obj, Enum());
}

/**
 * Convert the C++ scoped enum into the appropriate `enum.Enum`.
 *
 * For C++ scoped enums with `char` type the `enum.Enum` is expected to have
 * string values. Otherwise, the `enum.Enum` must have integral values.
 *
 * @note The scoped enum itself must be defined with the appropriate underlying
 *  type. Simply having `char` members is not enough if no underlying type is
 *  specified as the scoped enum's default underlying type is `int`.
 *
 * On error a Python exception is set and the returned object is empty.
 *
 * @note This function generally should *not* be used directly. It is intended
 *  for use from SWIG typemaps where the C++ and Python type names are known.
 *
 * @tparam E C++ scoped enum
 * @tparam N1 Length of fully-qualified Python module name
 * @tparam N2 Length of Python enum type name
 *
 * @param value C++ scoped enum value with corresponding `enum.Enum` instance
 * @param cxx_name C++ fully-qualified scoped enum type name
 * @param mod_name Python fully-qualified module name with the `enum.Enum` type
 * @param enum_name Python `enum.Enum` type name
 */
template <typename E, std::size_t N1, std::size_t N2>
requires (std::is_enum_v<E> && !std::is_convertible_v<E, int>)
py_object Enum(
  E value,
  std::string_view cxx_name,
  const fixed_string<N1>& mod_name,
  const fixed_string<N2>& enum_name) noexcept
{
  static_assert(N1, "Python module name is empty");
  static_assert(N2, "Python enum type name is empty");
  // import the module containing the enum
  // note: if the module rests in a package, this actually returns the package
  py_object mod{PyImport_ImportModule(mod_name)};
  if (!mod)
    return {};
  // retrieve the enum class type from the module
  py_object enum_class{PyObject_GetAttrString(mod, enum_name)};
  if (!enum_class)
    return {};
  // check that _cxx_type_name of enum class matches
  if (!check_cxx_type_name(enum_class, cxx_name))
    return {};
  // wrap value into Python based on underlying type
  // note: assumption is that _cxx_type_name match means enum is valid
  auto py_value = [value]
  {
    // create Python string of 1 character from enum value
    if constexpr (std::is_same_v<char, std::underlying_type_t<E>>) {
      char buf[2];
      buf[0] = static_cast<char>(value);
      buf[1] = '\0';
      return py_object{PyUnicode_FromStringAndSize(buf, 1)};
    }
    // note: assuming that sizeof(E) <= sizeof(long long)
    else
      return py_object{PyLong_FromLongLong(static_cast<long long>(value))};
  }();
  if (!py_value)
    return {};
  // invoke Enum class to create member from value
#if OA_PY_VERSION_IS(>=, 3, 9)
  return py_object{PyObject_CallOneArg(enum_class, py_value)};
#else
  // without PyObject_CallOneArg() we have to use Py_BuildValue()
  py_object tup{Py_BuildValue("(O)", py_value.get())};
  if (!tup)
    return {};
  return py_object{PyObject_CallObject(enum_class, tup)};
#endif  // !OA_PY_VERSION_IS(>=, 3, 9)
}
OA_GNU_WARNING_POP()

}  // namespace
}  // namespace enum_

namespace {

OA_GNU_WARNING_PUSH()
OA_GNU_WARNING_DISABLE(unused-function)
/**
 * Convert the `enum.Enum` back to its appropriate C++ scoped enum.
 *
 * For C++ scoped enums with `char` type the `enum.Enum` is expected to have
 * string values. Otherwise, the `enum.Enum` must have integral values.
 *
 * @note The scoped enum itself must be defined with the appropriate underlying
 *  type. Simply having `char` members is not enough if no underlying type is
 *  specified as the scoped enum's default underlying type is `int`.
 *
 * On error a Python exception is set and the returned optional is empty.
 *
 * @note This function generally should *not* be used directly. It is intended
 *  for use from SWIG typemaps where the C++ type name is explicitly given.
 *
 * @tparam E C++ scoped enum
 *
 * @param obj `enum.Enum` instance corresponding to the C++ scoped enum type
 * @param type C++ fully-qualified scoped enum type name
 */
template <typename E>
requires (std::is_enum_v<E> && !std::is_convertible_v<E, int>)
std::optional<E> to_scoped_enum(PyObject* obj, std::string_view type) noexcept
{
  // if not enum.Enum, error
  if (!enum_::IsEnum(obj)) {
    PyErr_SetString(
      PyExc_TypeError,
      fixed_string{
        OA_PRETTY_FUNCTION_NAME,
        ": incorrect type: input must be an enum.Enum instance"
      }
    );
    return {};
  }
  // if _cxx_type_name doesn't match, error
  if (!check_cxx_type_name(obj, type))
    return {};
  // _cxx_type_name matches so extract the value of the enum.Enum
  py_object value{PyObject_GetAttrString(obj, "value")};
  if (!value)
    return {};
  // branch based on underlying type
  // note: assumption is that _cxx_type_name match means enum is valid
  if constexpr (std::is_same_v<char, std::underlying_type_t<E>>) {
    // if str, the length must be 1, so we can map to char
    auto enum_value = to_utf8_view(value);
    if (!enum_value)
      return {};
    if (enum_value->size() != 1u) {
      PyErr_SetString(
        PyExc_ValueError,
        fixed_string{
          OA_PRETTY_FUNCTION_NAME,
          ": string enum values must have length 1 to map back to C++ char"
        }
      );
      return {};
    }
    // convert back to C++ enum via direct-initialization
    return E{enum_value->front()};
  }
  else {
    // extract int
    // note: we could switch conversion functions based on C++ type
    auto enum_value = PyLong_AsLong(value);
    if (PyErr_Occurred())
      return {};
    // forcibly convert back to C++ enum
    return E{static_cast<std::underlying_type_t<E>>(enum_value)};
  }
}
OA_GNU_WARNING_POP()

}  // namespace

}  // namespace oa
%}

/**
 * Macro for in/out typemaps mapping a C++ scoped enum to a Python enum class.
 *
 * Use of this typemap requires that the Python `enum.Enum` in question will be
 * exported in this SWIG module or from another SWIG module using the
 * `OA_EXPORT_ENUM_CLASS()` macro. This type map also provides a typecheck
 * typemap for supporting C++ overloading.
 *
 * @param cxx_type C++ fully-qualified scoped enum type
 * @param py_type Python fully-qualified `enum.Enum` subclass
 */
%define OA_TYPEMAP_ENUM_CLASS(cxx_type, py_type)
// input typemap mapping enum.Enum to scoped enum
%typemap(in) cxx_type {
  auto value = oa::to_scoped_enum<cxx_type>($input, #cxx_type);
  if (!value)
    SWIG_fail;
  $1 = *value;
}

// output typemap mapping scoped enum to enum.Enum
%typemap(out) cxx_type {
  // split qualified Python type into module + enum name at compile time
  constexpr auto mod_name = oa::fixed_string{#py_type}
    .substr<0u, oa::fixed_string{#py_type}.rfind('.')>();
  constexpr auto type_name = oa::fixed_string{#py_type}
    .substr<oa::fixed_string{#py_type}.rfind('.') + 1u>();
  // convert into corresponding enum.Enum
  $result = oa::enum_::Enum($1, #cxx_type, mod_name, type_name).release();
}

OA_OBJECT_TYPECHECK(cxx_type) {
  // FIXME: check_cxx_type_name() always sets exception if false. may want a
  // pure checking method that only sets Python exception on errors
  $1 = oa::enum_::IsEnum($input) && oa::check_cxx_type_name($input, #cxx_type);
}
%enddef  // OA_TYPEMAP_ENUM_CLASS(cxx_type, py_type)

// helper Python code to convert SWIG-wrapped scoped enum members to enum.Enum
// note: inline definition of the _export_enum_class() confused SWIG due to
// prior use of %{ and %} in the _export_enum_class() docstring
%pythoncode "oa/typemaps/scoped_enums.py"

/**
 * Macro for exporting a C++ scoped enum to Python as an enum class.
 *
 * This is macro that is needed to export a C++ scoped enum processed by SWIG
 * via `%include`, whose members have been turned into module-level members, as
 * a Python `enum.Enum`. Using this macro adds a `%pythoncode` block that calls
 * the `_export_enum_class()` function to convert all the SWIG-exported module
 * members into an `enum.Enum` with said members.
 *
 * Refer to the `_export_enum_class()` docstring for usage details.
 *
 * @note Ensure that this macro is used after any `%include` directives that
 *  are necessary for SWIG to export the members of any wrapped scoped enums.
 *
 * @param cxx_type C++ fully-qualified scoped enum type
 */
%define OA_EXPORT_ENUM_CLASS(cxx_type, ...)
%pythoncode %{
_export_enum_class(`cxx_type`, ##__VA_ARGS__)
%}
%enddef  // OA_EXPORT_ENUM_CLASS(cxx_type, ...)

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
py_object Path(const std::filesystem::path& path) noexcept
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
  else
    PyErr_SetString(
      PyExc_TypeError,
      fixed_string{
        OA_PRETTY_FUNCTION_NAME,
        ": incorrect type: input must be str or pathlib.Path"
      }
    );
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
  $result = oa::pathlib::Path($1).release();
}
