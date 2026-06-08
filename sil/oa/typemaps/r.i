/**
 * @file typemaps/r.i
 * @author Derek Huang
 * @brief C++ SWIG R typemaps
 * @copyright MIT License
 *
 * @note This SWIG module is not production quality and is mostly useful as an
 *  example of how to use the R internals to for C++ and `SEXP` interop. SWIG
 *  doesn't really do a good job of correctly registering all the C++ types
 *  as external pointers in R with the generated .R wrapper which makes any
 *  generated R bindings mostly unusable. However, these typemaps enable using
 *  `dyn.load()` on the shared library built from the SWIG-generated C++ to
 *  work, although doing `.Call("R_swig_<my function>", ...)` is unwieldy.
 */

#ifndef __cplusplus
#error "typemaps/r.i: SWIG must be run in C++ mode"
#endif  // __cplusplus

#ifndef SWIGR
#error "typemaps/r.i: can only be used in SWIG R mode"
#endif  // SWIGR

// TODO: need some thought on how to handle C++ exceptions given R's use of
// longjmp in Rf_error() which doesn't do the needed stack unwinding

// ensure that compilation doesn't break with R >=4.6.0 as SET_S4_OBJECT() is
// now hidden by default in the Rinternals.h header. ENABLE_LEGACY_NONAPI_FUNS
// can be defined to make these visible (at least temporarily). the solution
// is to use a newer SWIG version >= 4.5 after the commit into mainline:
//
// https://github.com/swig/swig/commit/0601b9ca9401aed5bcf1b018be922b15a8cee92a
//
// see also the corresponding PR: https://github.com/swig/swig/pull/3411
//
#if SWIG_VERSION < 0x040500
%begin %{
// for R_VERSION, R_Version()
#include <Rversion.h>
// if R version >= 4.6.0 enable legacy functions for SET_S4_OBJECT() visibility
#if R_VERSION >= R_Version(4, 6, 0)
#define ENABLE_LEGACY_NONAPI_FUNS
#endif  // R_VERSION >= R_Version(4, 6, 0)
%}
#endif  // SWIG_VERSION < 0x040500

// SEXP support code
%{
#include <filesystem>
#include <string_view>

namespace oa {
namespace {

/**
 * Convert a `std::string_view` to a scalar `STRSXP`.
 *
 * If the string view is empty `R_BlankScalarString` is returned.
 *
 * @param str String view
 */
auto to_sexp(std::string_view str) noexcept
{
  if (str.empty())
    return R_BlankScalarString;
  // construct STRSXP with only one CHARSXP (scalar string)
  return Rf_ScalarString(Rf_mkCharLen(str.data(), static_cast<int>(str.size())));
}

/**
 * Indicate if an R object is a scalar, or atomic, string.
 *
 * This simply checks for a string type and if the vector length is 1.
 *
 * @param v R object
 */
bool is_scalar_string(SEXP v) noexcept
{
  return Rf_isString(v) && (Rf_length(v) == 1);
}

}  // namespace
}  // namespace oa
%}

%include "oa/common.i"  // for OA_OBJECT_TYPECHECK()

/**
 * Typemap to convert a `std::string_view` into an R string.
 *
 * R has no related string view type so we need to return a character vector
 * instead. We also need to explicitly do a size check for `mkChar()` since
 * empty string views could have `nullptr` data.
 *
 * @note Every R object is a vector so a string is actually a length-1 vector.
 *  At the C level, the R string is a `STRSXP` with one `CHARSXP`.
 */
%typemap(out) std::string_view {
  $result = oa::to_sexp($1);
}

/**
 * Typemap to convert an R string into a `std::filesystem::path`.
 */
%typemap(in) std::filesystem::path {
  // if not an atomic R string, die
  // TODO: unsure yet how to propagate errors
  if (!oa::is_scalar_string($input))
    SWIG_fail;
  // create from underlying null-terminated string
  $1 = R_CHAR(STRING_ELT($input, 0));
}

/**
 * Typemap to convert an R string into a `std::filesystem::path`.
 *
 * This form takes a const reference so a temporary is required.
 */
%typemap(in) const std::filesystem::path& (std::filesystem::path path) {
  // same logic as with by-value creation
  // TODO: duplicated
  if (!oa::is_scalar_string($input))
    SWIG_fail;
  // create from underlying null-terminated string
  path = R_CHAR(STRING_ELT($input, 0));
  // note: reference typemaps always bind to pointers
  $1 = &path;
}

/**
 * Macro for a `std::filesystem::path` typecheck.
 *
 * This succeeds if the R object is an atomic string.
 *
 * @note Typechecks are only used to disambiguate overloads.
 *
 * @param type cvref-qualified `std::filesystem::path`
 */
%define OA_FILESYSTEM_PATH_CHECK(type)
OA_OBJECT_TYPECHECK(type) {
  $1 = oa::is_scalar_string($input);
}
%enddef  // OA_FILESYSTEM_PATH_CHECK(type)

OA_FILESYSTEM_PATH_CHECK(std::filesystem::path)
OA_FILESYSTEM_PATH_CHECK(const std::filesystem::path&)

/**
 * Typemap to convert a `std::filesystem::path` into an R string.
 *
 * If the path is empty we return an empty `STRSXP`.
 */
%typemap(out) std::filesystem::path {
  $result = oa::to_sexp($1.string());
}
