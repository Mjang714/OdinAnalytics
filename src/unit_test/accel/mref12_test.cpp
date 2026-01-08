/**
 * @file mref12_test.cpp
 * @author Derek Huang
 * @brief mref12.h unit tests
 * @copyright MIT License
 */

#include "oa/accel/mref12.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include <sstream>
#include <tuple>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "oa/accel/xl_ops.h"  // for xlref12 operator==

namespace {

/**
 * `mref12` test fixture.
 */
class MultiRef12Test : public ::testing::Test {};

/**
 * Test constructing an empty `mref12`.
 */
TEST_F(MultiRef12Test, MakeEmptyTest)
{
  oa::accel::mref12 ref;
  EXPECT_EQ(0, ref.size());
  EXPECT_EQ(nullptr, ref.release());  // nothing is managed
}

/**
 * Test constructing `mref12` with a given size.
 */
TEST_F(MultiRef12Test, MakeSizedTest)
{
  constexpr auto n_refs = 5u;
  oa::accel::mref12 ref{n_refs};
  EXPECT_EQ(n_refs, ref.size());
}

/**
 * Test constructing `mref12` from several `xlref12` objects.
 *
 * This also tests `operator[]` indexing.
 */
TEST_F(MultiRef12Test, FromRefListTest)
{
  oa::accel::mref12 ref{{1, 1, 4, 4}, {2, 3, 4, 5}, {2, 10, 0, 5}};
  EXPECT_EQ(3u, ref.size());
  // smoke test some values
  EXPECT_EQ(ref[0].colFirst, ref[1].colFirst);
  EXPECT_EQ(ref[1].rwFirst, ref[2].rwFirst);
}

/**
 * Test `mref12` copy testing.
 *
 * This also tests construction from an initializer list + `operator==`.
 */
TEST_F(MultiRef12Test, CopyTest)
{
  oa::accel::mref12 ref1{{1, 2, 4, 5}, {1, 6, 2, 4}, {10, 24, 0, 16}};
  auto ref2 = ref1;
  // note: mref12 implements operator==
  EXPECT_THAT(ref1, ::testing::ContainerEq(ref2));
}

/**
 * Test `mref12` range-for test.
 *
 * This also tests construction with a given size + iterators.
 */
TEST_F(MultiRef12Test, RangeForTest)
{
  // target cell reference
  constexpr xlref12 tgt{0, 0, 1, 1};
  // populate
  oa::accel::mref12 ref{3};
  for (auto& v : ref)
    v = tgt;
  // all values should be equal now
  // note: this is only possible since mref12 provides value_type
  EXPECT_THAT(ref, ::testing::Each(tgt));
}

/**
 * Test `mref12` string formatting.
 *
 * This indirectly tests the `operator<<` for the `xlref12` + list init.
 */
TEST_F(MultiRef12Test, StreamFormatTest)
{
  oa::accel::mref12 ref{
    {1, 6, 4, 18},  // (1, 4) ... (6, 18)
    {4, 4, 6, 6},   // (4, 6)
    {2, 9, 5, 5},   // (2, 5) ... (9, 5)
    {6, 6, 0, 10}   // (6, 0) ... (6, 10)
  };
  // stream
  std::stringstream ss;
  ss << ref;
  // check
  EXPECT_EQ(
    "[(1, 4) ... (6, 18), "
      "(4, 6), "
      "(2, 5) ... (9, 5), "
      "(6, 0) ... (6, 10)]",
    ss.str()
  );
}

}  // namespace
