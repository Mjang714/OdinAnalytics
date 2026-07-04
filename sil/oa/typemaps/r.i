/**
 * @file typemaps/r.i
 * @author Derek Huang
 * @brief C++ SWIG R typemaps
 * @copyright MIT License
 */

#ifndef __cplusplus
#error "typemaps/r.i: SWIG must be run in C++ mode"
#endif  // __cplusplus

#ifndef SWIGR
#error "typemaps/r.i: can only be used in SWIG R mode"
#endif  // SWIGR

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

// R support code
%{
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string_view>

namespace oa {
namespace {

/**
 * Extract the given 16-bit word from `std::uint64_t`.
 *
 * The first word corresponds to the uppermost 16 bits and the last word
 * corresponds to the lowermost 16 bits.
 *
 * @tparam I Word index in `{1, ... 4}`
 *
 * @param v Value to extract 16-bit word from
 */
template <std::size_t I>
constexpr auto word(std::uint64_t v) noexcept
{
  static_assert(I >= 1u && I <= 4, "I must be in {1, ... 4}");
  return static_cast<std::uint16_t>((v >> (16u * (4u - I))) & 0xFFFF);
}

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

%include "exception.i"  // for SWIG_exception()
%include "oa/common.i"  // for OA_OBJECT_TYPECHECK()

// STL definition mocks so SWIG can correctly generate R wrapper classes.
//
// by default SWIG treats any non-native C type as something that needs to be
// wrapped in an S4 class. however, classes like std::filesystem::path, that
// don't have available SWIG interfaces, never end up getting the R class
// wrapper code generated, so attempting to call functions that return these
// types results in errors like ""_p_std__filesystem__path" is not a defined
// class" being emitted by the R interpreter.
//
// although we could mock out the STL class definitions ourselves, it's nice to
// have mappings from C++ types to native types like we do for Python. to do
// so, we instead tell SWIG to treat various STL types as if they were native
// C/C++ types, but then we also use custom typemaps so that the generated C++
// code can do the appropriate SEXP to C++ type conversion.
//
namespace std {

using uint16_t = unsigned short;
using uint32_t = unsigned int;

/**
 * `std::uint64_t` SWIG mock.
 *
 * R only has 32-bit signed integers (actually, they internally use `int`, so
 * technically the width is machine dependent). Therefore, `uint64_t` has to be
 * broken down into 4 x 16-bit words so each can be correctly unsigned.
 *
 * This class is provided only to induce SWIG's generation of S4 classes for
 * the `std::uint64_t` and of course doesn't correspond to the C++ type. The
 * `%extend` block manipulates the `std::uint64_t*` created by SWIG to allow
 * returning the 4 different 16-bit words using separate member functions.
 *
 * @note This *must* be a `class`, not a `struct`, as otherwise SWIG will
 *  generate extra code for copying what it thinks is a `struct` to R. We only
 *  want SWIG to generate the `%extend` methods, so we must use `class`.
 *
 * @par
 *
 * @note SWIG is good at wrapping classes using R's `externalptr` so we don't
 *  need to have any additional custom typemaps.
 */
class uint64_t {
public:
  %extend {
    /**
     * Return the uppermost 16 bits of the `std::uint64_t`.
     */
    uint16_t w1() const noexcept { return oa::word<1>(*$self); }

    /**
     * Return the second uppermost 16 bits of the `std::uint16_t`.
     */
    uint16_t w2() const noexcept { return oa::word<2>(*$self); }

    /**
     * Return the third uppermost 16 bits of the `std::uint16_t`.
     */
    uint16_t w3() const noexcept { return oa::word<3>(*$self); }

    /**
     * Return the lowest 16 bits of the `std::uint16_t`.
     */
    uint16_t w4() const noexcept { return oa::word<4>(*$self); }
  }
};

using string = const char*;
using string_view = const char*;

namespace filesystem { using path = const char*; }  // namespace filesystem

}  // namespace std

/**
 * Typemap to convert a `std::string` into an R string.
 *
 * This copies the contents of the string into a new R character vector.
 *
 * @note Every R object is a vector so a string is actually a length-1 vector.
 *  At the C level, the R string is a `STRSXP` with one `CHARSXP`.
 */
%typemap(out) std::string {
  $result = oa::to_sexp($1);
}

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
    SWIG_exception(SWIG_RuntimeError, "$input is not a scalar R string");
  // create from underlying null-terminated string
  $1 = R_CHAR(STRING_ELT($input, 0));
}

/**
 * Typemap to convert an R string into a `std::filesystem::path`.
 *
 * This form takes a const reference so a temporary is required.
 *
 * @note The locally-allocated `pth` variable is named such to allow us to call
 *  `~path()` explicitly. Otherwise, SWIG ends up renaming `path` to `path1`,
 *  but also ends up renaming `~path()` to `~path1()`, causing a compile error.
 */
%typemap(in) const std::filesystem::path& (std::filesystem::path pth) {
  // similar logic as with by-value creation
  if (!oa::is_scalar_string($input)) {
    // explicitly destroy pth because after SWIG_exception() jumps using
    // SWIG_fail Rf_error() is called, which will longjmp and therefore result
    // in any stack-allocated C++ objects not being destructed properly
    pth.~path();
    SWIG_exception(SWIG_RuntimeError, "$input is not a scalar R string");
  }
  // create from underlying null-terminated string
  pth = R_CHAR(STRING_ELT($input, 0));
  // note: reference typemaps always bind to pointers
  $1 = &pth;
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
