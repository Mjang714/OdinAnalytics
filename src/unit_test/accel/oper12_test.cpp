/**
 * @file oper12_test.cpp
 * @author Derek Huang
 * @brief oper12.h unit tests
 * @copyright MIT License
 */

#include "oa/accel/oper12.h"

// TODO: remove once we have a proper to<T>() implementation
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include <string>

#include <gtest/gtest.h>

#include "oa/accel/xl_ops.h"  // for xlref12 operator==

namespace {

/**
 * `oper12` test fixture.
 */
class Oper12Test : public ::testing::Test {};

/**
 * Test that `xlerr` construction works as expected.
 */
TEST_F(Oper12Test, MakeErrTest)
{
  constexpr auto x = oa::accel::xlerr::div0;
  oa::accel::oper12 val{x};
  ASSERT_TRUE(val.error()) << "val does not contain an xlerr";
  EXPECT_EQ(x, *val.error());
}

/**
 * Test that `bool` construction works as expected.
 */
TEST_F(Oper12Test, MakeBoolTest)
{
  constexpr bool x = true;
  oa::accel::oper12 val{x};
  ASSERT_TRUE(val.type() == oa::accel::xltype::bool_) << "val is not bool_";
  // TODO: add conversion for value inspection
  EXPECT_EQ(x, !!val.value()->val.xbool);
}

/**
 * Test that `int` construction works as expected.
 */
TEST_F(Oper12Test, MakeIntTest)
{
  constexpr int x = 100;
  oa::accel::oper12 val{x};
  ASSERT_TRUE(val.type() == oa::accel::xltype::int_) << "val is not int_";
  // TODO: add conversion for value inspection
  EXPECT_EQ(x, val.value()->val.w);
}

/**
 * Test that `double` construction works as expected.
 */
TEST_F(Oper12Test, MakeDoubleTest)
{
  constexpr double x = 2.;
  oa::accel::oper12 val{x};
  ASSERT_TRUE(val.type() == oa::accel::xltype::num) << "val is not a number";
  // TODO: add conversion for value inspection
  EXPECT_DOUBLE_EQ(x, val.value()->val.num);
}

/**
 * Test that `std::string` construction works as expected.
 */
TEST_F(Oper12Test, MakeStringTest)
{
  std::string str{"hello world"};
  oa::accel::oper12 val{str};
  ASSERT_TRUE(val.type() == oa::accel::xltype::str) << "val is not a string";
  // owns extra memory
  EXPECT_TRUE(val.owning());
  // TODO: add conversion for value inspection
  // Excel 12 uses wide strings and size is in first code point
  EXPECT_EQ(
    (L"\013" + std::wstring{str.begin(), str.end()}),
    std::wstring_view{val.value()->val.str}
  );
}

/**
 * Test that `std::wstring` construction works as expected.
 */
TEST_F(Oper12Test, MakeWstringTest)
{
  std::wstring str{L"hello world"};
  oa::accel::oper12 val{str};
  ASSERT_TRUE(val.type() == oa::accel::xltype::str) << "val is not a string";
  // owns extra memory
  EXPECT_TRUE(val.owning());
  // TODO: add conversion for value inspection
  // Excel 12 wide string size is in first code point
  EXPECT_EQ(L"\013" + str, std::wstring_view{val.value()->val.str});
}

/**
 * Test that single-cell reference construction works as expected.
 */
TEST_F(Oper12Test, MakeSingleRefTest)
{
  xlref12 ref{1, 1, 45, 45};
  oa::accel::oper12 val{ref};
  ASSERT_TRUE(val.type() == oa::accel::xltype::sref) << "val is not an sref";
  // note: count is always 1
  EXPECT_EQ(1, val.value()->val.sref.count);
  EXPECT_EQ(ref, val.value()->val.sref.ref);
}

// TODO: add tests for multi-ref

/**
 * Test that a nil `oper12` works as expected.
 */
TEST_F(Oper12Test, MakeNilTest)
{
  EXPECT_EQ(oa::accel::xltype::nil, oa::accel::oper12::nil().type());
}

/**
 * Test that a missing `oper12` works as expected.
 */
TEST_F(Oper12Test, MakeMissingTest)
{
  EXPECT_EQ(oa::accel::xltype::missing, oa::accel::oper12::missing().type());
}

}  // namespace
