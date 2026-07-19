/**
 * @file scope_exit.h
 * @author Derek Huang
 * @brief C++ header for a scoping guard
 * @copyright MIT License
 */

#ifndef OA_SCOPE_EXIT_H_
#define OA_SCOPE_EXIT_H_

#include <functional>
#include <utility>

namespace oa {

/**
 * RAII object that ensures a callable is invoked before scope exit.
 *
 * This is similar to `std::experimental::scope_exit` buts for simplicity and
 * to enable type erasure uses `std::function` at the cost of some performance.
 * In the future this class might be rewritten to be more similar to
 * `std::experimental::scope_exit` to avoid any polymorphic overhead.
 */
class scope_exit {
public:
  /**
   * Ctor.
   *
   * @tparam F Nullary invocable
   *
   * @param f Nullary invocable to construct from
   */
  template <typename F>
  scope_exit(F&& f) : f_{std::forward<F>(f)} {}

  /**
   * Dtor.
   *
   * Invokes the contained callable and swallows all exceptions.
   */
  ~scope_exit() try
  {
    if (f_)
      f_();
  }
  catch (...) {}

  /**
   * Drop the contained callable.
   *
   * On scope exit nothing will be done when the `scope_exit` is destroyed.
   */
  void release() noexcept
  {
    f_ = nullptr;
  }

  /**
   * Indicate if the scope guard is active or not.
   */
  explicit operator bool() const noexcept
  {
    return !!f_;
  }

private:
  std::function<void()> f_;
};

}  // namespace oa

#endif  // OA_SCOPE_EXIT_H_
