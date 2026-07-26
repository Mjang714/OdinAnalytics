/**
 * @file benchmark_main.cpp
 * @author Derek Huang
 * @brief C++ source for the Google benchmark library's main
 * @copyright MIT License
 *
 * This stub implementation is provided to ensure that LTO version mismatches
 * are avoided when using Ubuntu-packaged versions of the Google benchmark
 * library's benchmark_main `main()`. The libbenchmark-dev 1.6.1-1 provided by
 * Ubuntu 22.04 contains a libbenchmark_main.a static library stub that was
 * compiled using an older, incompatible LTO version.
 *
 * Whether or not LTO is used with what is essentially just a `main()` stub
 * doesn't seem to matter much so we provide our own stub containing the
 * `BENCHMARK_MAIN()` macro. This allows us to continue using whichever
 * upstream Google benchmark library provided, whether static or shared, and
 * without any need to worry about LTO incompatibilities.
 */

#include <benchmark/benchmark.h>

BENCHMARK_MAIN();
