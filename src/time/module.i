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

// ensure all class + enum types are in PascalCase
%rename("%(camelcase)s", %$isclass) "";
%rename("%(camelcase)s", %$isenum) "";
// ensure all function + enum members are snake_case for Python
// note: the enum members will be made uppercase later when exported
%rename("%(undercase)s", %$isfunction) "";
%rename("%(undercase)s", %$isenumitem) "";

// ignore unwrappable operators
namespace oa::time {

// note: explicitly ignored using %ignore to suppress the emitted SWIG warning
%ignore operator<<;
%ignore operator+;
%ignore operator-;

}  // namespace oa::time

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

// oa::time::Tenor %ignore and %extend directives
namespace oa::time {

// note: SWIG doesn't support nested structs
%ignore Tenor::GroupLess;
// ignore deprecated members
// note: in particular, GetValues() has no corresponding typemap for its return
// type, so we end up leaking a std::pair<int, Tenors> on every call
%ignore Tenor::FlipSign;
%ignore Tenor::GetValues;

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

%include "oa/time/tenor.h"

// oa::time::Date %ignore and %extend directives
namespace oa::time {

// no typecheck + typemap for std::chrono::system_clock::time_point
//
// note:
//
// SWIG is not intelligent enough to realize that Date::time_point is an alias
// to std::chrono::system_clock::time_point. we must use the time_point type
// alias directly, otherwise SWIG generates a wrapper regardless
//
%ignore Date::Date(const time_point&);
// avoid static IsLeap(int) clobbering IsLeap() member in wrapper
%rename(is_leap_year) Date::IsLeap(int);
// avoid static GetDOWInt(int) clobbering GetDOWInt() member in wrapper + use a
// nice shorter name for GetDOWInt() member function
%rename(dow_int) Date::GetDOWInt(int);
%rename(dow) Date::GetDOWInt() const;
// member function shadows static function of different call signature + we
// don't have any typecheck + typemaps for std::chrono:: types
%ignore Date::ConvertToTimePt;
// ignore deprecated functions
%ignore Date::ToString;
%ignore Date::GetJulian;
%ignore Date::AddTenor;
%ignore Date::SubTenor;
// ignore gregorian() as we don't have a std::tuple<int, int, int> typemap
// TODO: consider adding tuple typemaps based
%ignore Date::gregorian;

// Python member functions for the Date class
%extend Date {
  /**
   * Add the given number of days to the `Date`.
   *
   * This replaces the unwrappable non-member `operator+(const Date&, int)`.
   */
  Date __add__(int days)
  {
    return *$self + days;
  }

  /**
   * Subtract the give number of days from the `Date`.
   *
   * This replaces the unwrappable non-member `operator-(const Date&, int)`.
   */
  Date __sub__(int days)
  {
    return *$self - days;
  }

  /**
   * Return the hash value of the `Date`.
   *
   * Since each date is based on a Julian day number we can just return the
   * Julian day number itself as the hash value to let `Date` be hashable.
   */
  Py_ssize_t __hash__()
  {
    return $self->julian();
  }

  /**
   * Return the string representation of the `Date`.
   *
   * This is implemented using the `operator<<` overload for `Date`.
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

// note: date.h must be processed after tenor.h since we need SWIG to first
// recognize Tenor in order to correctly process Date interfaces using Tenor
%include "oa/time/date.h"
