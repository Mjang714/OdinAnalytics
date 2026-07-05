/**
 * @file isdigit_bench.cpp
 * @author Derek Huang
 * @brief C++ program benchmarking `oa::is_digit()` vs. `std::isdigit()`
 * @copyright MIT License
 *
 * @note For accurate measurements compile with optimizations turned on. Also,
 *  if compiling with MSVC, ensure that the non-debug CRT library is used.
 */

#include <cctype>
#include <string_view>

#include <benchmark/benchmark.h>

#include "oa/string.h"
#include "oa/warnings.h"

// for MSVC, indicate if debug C runtime library is being used
#if defined(_MSC_VER) && defined(_DEBUG)
OA_WARNING("using debug C runtime library")
#endif  // defined(_MSC_VER) && defined(_DEBUG)

// emit informational message if compiling without optimizations
// note: for GCC, __OPTIMIZE__ is defined if compiling with optimizations; for
// other compilers we just check if NDEBUG is defined
#if (defined(__GNUC__) && !defined(__OPTIMIZE__)) || !defined(NDEBUG)
OA_WARNING("compiling without optimization")
#endif  // (defined(__GNUC__) && !defined(__OPTIMIZE__)) || !defined(NDEBUG)

namespace {

/**
 * Benchmarking fixture with the string literal to count digits for.
 *
 * This lets us use the `BENCHMARK_F()` macro for less repetition.
 */
class DigitsFixture : public benchmark::Fixture {
public:
  // 37 digits
  static constexpr std::string_view digit_str =
    "s890duf23uf0sjadjfj932fu239f209u2904892390jejs0fj90d0as9f8sdyf19ghf819hf9";
};

/**
 * Safe version of `std::isdigit()` for use with standard `char`.
 *
 * @note When not compiling with optimization turned on this wrapper adds
 *  function call overhead due to the lack of inlining. With optimization,
 *  however, using this wrapper helds avoid a slowdown caused by the Google
 *  benchmark library's `DoNotOptimize()` memory barrier.
 */
bool is_digit(char c)
{
  return !!std::isdigit(static_cast<unsigned char>(c));
}

/**
 * Count the digits in `digit_str` using `std::isdigit()`.
 */
BENCHMARK_F(DigitsFixture, StdIsDigit)(benchmark::State& state)
{
  for (auto _ : state) {
    auto n = 0u;
    for (auto c : digit_str)
      benchmark::DoNotOptimize(n += is_digit(c));
  }
}

/**
 * Count the digits in `digit_str` using `oa::is_digit()`.
 */
BENCHMARK_F(DigitsFixture, OaIsDigit)(benchmark::State& state)
{
  for (auto _ : state) {
    auto n = 0u;
    for (auto c : digit_str)
      benchmark::DoNotOptimize(n += oa::is_digit(c));
  }
}

}  // namespace

// note:
//
// older Google benchmark library versions on Ubuntu, e.g. 1.6.1, provide a
// static benchmark_main that is built with an incorrect LTO version. linking
// against the benchmark library itself is not an issue, however.
//
// unfortunately, this means that if using an upstream benchmark installation,
// it is safest to use the BENCHMARK_MAIN() macro explicitly, which is why we
// provide our own main() stub containing the BENCHMARK_MAIN() macro.
//
