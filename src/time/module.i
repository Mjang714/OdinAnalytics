/**
 * @file time/module.i
 * @author Derek Huang
 * @brief SWIG C++ module for the time library
 * @copyright MIT License
 */

%define MODULE_DOCSTRING
"SWIG module for the OA time library.

This module provides wrappers for oa_time library functions and types."
%enddef  // MODULE_DOCSTRING

// note: docstring option only has an effect for Python wrappers
%module(docstring=MODULE_DOCSTRING) oa_time

#ifdef SWIGPYTHON
// ensure release Python libraries are linked against
%begin %{
#ifdef _MSC_VER
#define SWIG_PYTHON_INTERPRETER_NO_DEBUG
#endif  // _MSC_VER
%}
#endif  // SWIGPYTHON

%{
#include <sstream>
#include <string>
#include <utility>

#include "oa/time/date.h"
#include "oa/time/enums.h"
#include "oa/time/tenor.h"
%}

%include "oa/common.i"
%include "oa/typemaps.i"

OA_HANDLE_EXCEPTIONS

// process oa/dllexport.h for OA_TIME_API
%import "oa/dllexport.h"

namespace oa::time {

// note: explicitly ignored using %ignore to suppress the emitted SWIG warning
%ignore operator<<;
// note: we don't have a typecheck + typemap to disambiguate the overloads
%ignore Date::Date(const std::chrono::system_clock::time_point&);
// note: SWIG doesn't support nested structs
%ignore Tenor::GroupLess;

// Python member functions for the Tenor class
// note: auto supported as return type in SWIG 4.1
%extend Tenor {
  /**
   * Return the hash value of the `Tenor` object.
   *
   * This returns the value from `hash()` cast to a `Py_ssize_t`, which is
   * necessary to prevent the Python `hash()` built-in from truncating the
   * return value. Therefore, `tenor.hash()` will return a different value
   * from `tenor.__hash__()` and `hash(tenor)`.
   *
   * It is more important for `tenor.__hash__()` and `hash(tenor)` to be
   * consistent; if we returned a `std::size_t` directly back to Python, then
   * `tenor.__hash__()` and `tenor.hash()` would be the same, but
   * `hash(tenor)` would yield a different value.
   */
  Py_ssize_t __hash__()
  {
    return static_cast<Py_ssize_t>($self->hash());
  }

  /**
   * Return the string representation of the `Tenor`.
   *
   * This is implemented using the `operator<<` overload for `Tenor`.
   */
  std::string __repr__()
  {
    std::stringstream ss;
    ss << *$self;
    // note: in C++20 this saves a string copy by using the ref-qualified
    // overload of str() that move-constructs from the internal string
    return std::move(ss).str();
  }
}

}  // namespace oa::time

// ensure all class + enum types are in PascalCase
%rename("%(camelcase)s", %$isclass) "";
%rename("%(camelcase)s", %$isenum) "";
// ensure all enum members are in snake_case for Python (will be made uppercase)
%rename("%(undercase)s", %$isenumitem) "";

%include "oa/time/enums.h"

// define typemaps for C++ scoped enums to Python enum.Enum
OA_TYPEMAP_ENUM_CLASS(oa::time::Weekdays, oa_time.Weekdays)
OA_TYPEMAP_ENUM_CLASS(oa::time::Months, oa_time.Months)
OA_TYPEMAP_ENUM_CLASS(oa::time::Tenors, oa_time.Tenors)
OA_TYPEMAP_ENUM_CLASS(oa::time::AdjRule, oa_time.AdjRule)
OA_TYPEMAP_ENUM_CLASS(oa::time::DayCountRule, oa_time.DayCountRule)

// export C++ scoped enums as Python enum.Enum
//
// note:
//
// OA_EXPORT_ENUM_CLASS() must be called *after* all %include directives
// defining the scoped enums whose members have been wrapped into module-level
// members by SWIG. we also have strip_prefix="k_" because of the previous enum
// item %rename directive changing e.g. kWeeks into k_weeks
//
OA_EXPORT_ENUM_CLASS(oa::time::Weekdays, strip_prefix="k_")
OA_EXPORT_ENUM_CLASS(oa::time::Months, strip_prefix="k_")
OA_EXPORT_ENUM_CLASS(oa::time::Tenors, strip_prefix="k_")
OA_EXPORT_ENUM_CLASS(oa::time::AdjRule, strip_prefix="k_")
OA_EXPORT_ENUM_CLASS(oa::time::DayCountRule, strip_prefix="k_")

// TODO: add wrapping for the Date class after the Tenor class is done
// TODO: need to provide typechecks for oa::time::Tenors for Tenor(int, Tenors)
%include "oa/time/tenor.h"
