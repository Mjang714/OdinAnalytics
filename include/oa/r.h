/**
 * @file r.h
 * @author Derek Huang
 * @brief C++ header for R support
 * @copyright MIT License
 */

#ifndef OA_R_H_
#define OA_R_H_

#include <utility>

// avoid <name> macros being defined for Rf_<name> functions
#ifndef R_NO_REMAP
#define R_NO_REMAP
#endif  // R_NO_REMAP
#include <R.h>
#include <Rinternals.h>

namespace oa {

/**
 * RAII type for automatically protecting and unprotecting an `SEXP`.
 *
 * This type functions like a `std::unique_ptr<T>` for R `SEXP` objects and is
 * useful for ensuring that protect/unprotect calls are matched within scope.
 * Furthermore, this is useful when writing SWIG out typemaps, as the
 * `sexp` can be used to keep `$result` protected until return.
 *
 * @warning Calling `Rf_error()` or any other function to `longjmp` out of a
 *  frame that has automatic scope `sexp` objects holding ownership will likely
 *  result in memory leaks due to loss of protect/unprotect balance.
 */
class sexp {
public:
  /**
   * Default ctor.
   *
   * Creates an empty `sexp` with `nullptr` and no ownership.
   */
  sexp() noexcept : obj_{} {}

  /**
   * Ctor.
   *
   * Takes ownership of the `SEXP` by calling `Rf_protect()`.
   *
   * @param obj `SEXP` to protect from garbage collection
   */
  explicit sexp(SEXP obj) noexcept : obj_{Rf_protect(obj)} {}

  /**
   * Move ctor.
   */
  sexp(sexp&& other) noexcept
  {
    from(std::move(other));
  }

  /**
   * Move assignment operator.
   */
  auto& operator=(sexp&& other) noexcept
  {
    unprotect();
    from(std::move(other));
    return *this;
  }

  /**
   * Dtor.
   *
   * Unprotects the `SEXP` if it is not `nullptr`.
   */
  ~sexp()
  {
    unprotect();
  }

  /**
   * Return the `SEXP` pointer.
   */
  auto get() const noexcept
  {
    return obj_;
  }

  /**
   * Release ownership of the `SEXP` pointer.
   *
   * @warning The caller is now responsible for a `Rf_unprotect(1)` call after
   *  calling `release()` as the `SEXP` is still protected.
   */
  auto release() noexcept
  {
    auto obj = obj_;
    obj_ = nullptr;
    return obj;
  }

  /**
   * Implicitly convert to `SEXP` for interop with R functions.
   */
  operator SEXP() const noexcept
  {
    return obj_;
  }

private:
  SEXP obj_;

  /**
   * Move-initialize from another `sexp`.
   *
   * On return the moved-from `sexp` holds `nullptr` and no ownership.
   */
  void from(sexp&& other) noexcept
  {
    obj_ = other.obj_;
    other.obj_ = nullptr;
  }

  /**
   * Unprotect the `SEXP` if it is not `nullptr`.
   *
   * @note This clashes with the `unprotect` macro defined when `R_NO_REMAP` is
   *  not defined. In general it's better to define `R_NO_REMAP` and just use
   *  the prefixed functions for clarity and to avoid clashes.
   */
  void unprotect() noexcept
  {
    if (obj_)
      Rf_unprotect(1);
  }
};

}  // namespace oa

#endif  // OA_R_H_
