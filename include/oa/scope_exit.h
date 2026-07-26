/**
 * @file scope_exit.h
 * @author Derek Huang
 * @brief C++ header for a scoping guard
 * @copyright MIT License
 */

#ifndef OA_SCOPE_EXIT_H_
#define OA_SCOPE_EXIT_H_

#include <concepts>
#include <functional>
#include <type_traits>
#include <utility>

namespace oa {

/**
 * RAII object that ensures a callable is invoked before scope exit.
 *
 * This is essentially a `std::experimental::scope_exit` implementation with
 * static typing and a small size optimization for lvalue references or any
 * callables that are the size of `void*` or smaller.
 *
 * @tparam F Nullary invocable
 */
template <std::invocable F>
class scope_exit {
public:
  /**
   * Union type with small size optimization for `F`.
   *
   * `p_` is used for `F*` while `o_` is used for an `F` instance if `F` is
   * `sizeof(void*)` or smaller, i.e. the size of a pointer or smaller. This
   * allows us to do some small size optimizations for function objects without
   * much state that may be smaller than a `void*`.
   */
  union state {
    void* p_;                // F* if sizeof(F) > sizeof(void*)
    char o_[sizeof(void*)];  // F if sizeof(F) <= sizeof(void*)
  };

  /**
   * Indicate whether small size optimization will be used with `F`.
   */
  static constexpr bool use_sso = (sizeof(F) <= sizeof(state::o_));

  /**
   * Ctor.
   *
   * This constructs from an lvalue and will store just a pointer to `f`.
   * Therefore, no deleter needs to be run when the `scope_exit` is destroyed.
   *
   * @param f Nullary invocable to construct from
   */
  scope_exit(F& f) noexcept
    : u_{.p_ = &f},
      // invoke u_.p_ after casting to F*
      call_{[](state& u) { std::invoke(*static_cast<F*>(u.p_)); }},
      // no deleter since u_.p_ is active and it is a F*
      del_{}
  {}

  /**
   * Ctor.
   *
   * This move-constructs from an rvalue, using placement `new` if `f` is the
   * size of a pointer or smaller, and heap-allocating otherwise. The deleter
   * is appropriately defined for each case separately.
   *
   * @param f Nullary invocable to construct from
   */
  scope_exit(F&& f) noexcept(use_sso && std::is_nothrow_move_constructible_v<F>)
  {
    // f fits in the u_.o_ buffer
    if constexpr (use_sso) {
      // placement new for small size optimization
      new(u_.o_) F{std::move(f)};
      // invoke u_.o_ after casting to F*
      call_ = [](state& u) { std::invoke(*reinterpret_cast<F*>(u.o_)); };
      // invoke ~F() on u_.o_
      del_ = [](state& u) { reinterpret_cast<F*>(u.o_)->~F(); };
    }
    // f larger than u_.o_ buffer
    else {
      // heap allocation (possibly-throwing)
      u_.p_ = new F{std::move(f)};
      // invoke u_.p_ after casting to F*
      call_ = [](state& u) { std::invoke(*static_cast<F*>(u.p_)); };
      // invoke ~F() on u_.p_
      del_ = [](state& u) { static_cast<F*>(u.p_)->~F(); };
    }
  }

  /**
   * Deleted copy ctor.
   */
  scope_exit(const scope_exit&) = delete;

  /**
   * Deleted move ctor.
   */
  scope_exit(scope_exit&&) = delete;

  /**
   * Dtor.
   *
   * Invokes and deletes the contained callable, swallowing any exceptions.
   */
  ~scope_exit()
  {
    // invoke
    try {
      if (call_)
        call_(u_);
    }
    catch (...) {}
    // delete
    if (del_)
      del_(u_);
  }

  /**
   * Drop the contained callable.
   *
   * On scope exit nothing will be done when the `scope_exit` is destroyed. If
   * the deleter is not `nullptr` it is invoked to call `~F()`.
   */
  void release() noexcept
  {
    // reset call_
    call_ = nullptr;
    // invoke + reset deleter if non-null
    if (del_) {
      del_(u_);
      del_ = nullptr;
    }
  }

  /**
   * Indicate if the scope guard is active or not.
   */
  explicit operator bool() const noexcept
  {
    return !!call_;
  }

private:
  state u_;               // small-size-optimized union for the callable
  void (*call_)(state&);  // invokes u_.p_ or u_.o_
  void (*del_)(state&);   // deletes u_.o_
};

}  // namespace oa

#endif  // OA_SCOPE_EXIT_H_
