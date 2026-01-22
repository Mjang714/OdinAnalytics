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
#include <utility>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "oa/accel/enums.h"
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
 * Test that string literal construction works as expected.
 */
TEST_F(Oper12Test, MakeCStringTest)
{
  // note: using character array so we can use begin() and end()
  constexpr const char str[] = "hello world";
  oa::accel::oper12 val{str};
  ASSERT_TRUE(val.type() == oa::accel::xltype::str) << "val is not a string";
  // owns extra memory
  EXPECT_TRUE(val.owning());
  // TODO: add conversion for value inspection
  EXPECT_EQ(
    // note: subtract one from end(str) to avoid null terminator
    (L"\013" + std::wstring{std::begin(str), std::end(str) - 1u}),
    // note: use sizeof str because Excel string has prepended size
    (std::wstring_view{val.value()->val.str, sizeof str})
  );
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
    (std::wstring_view{val.value()->val.str, str.size() + 1u})
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
  EXPECT_EQ(
    L"\013" + str,
    (std::wstring_view{val.value()->val.str, str.size() + 1u})
  );
}

/**
 * Test that `std::wstring` copy works as expected.
 */
TEST_F(Oper12Test, CopyWStringTest)
{
  std::wstring str{L"hello world"};
  oa::accel::oper12 v1{str};
  auto v2 = v1;
  ASSERT_TRUE(v1.type() == v2.type());
  EXPECT_EQ(v1.owning(), v2.owning());
  // TODO: add conversion for value inspection
  // note: Excel strings have size prepended into first character
  EXPECT_EQ(
    (std::wstring_view{v1.value()->val.str, str.size() + 1u}),
    (std::wstring_view{v2.value()->val.str, str.size() + 1u})
  );
}

/**
 * Test that `std::wstring` move works as expected.
 */
TEST_F(Oper12Test, MoveWStringTest)
{
  std::wstring str{L"hello world"};
  oa::accel::oper12 v1{str};
  auto v2 = std::move(v1);
  // v1 has no value now
  ASSERT_FALSE(v1);
  // v2 will have the contents
  ASSERT_TRUE(v2.type() == oa::accel::xltype::str);
  // TODO: add conversion for value inspection
  EXPECT_EQ(
    L"\013" + str,
    (std::wstring_view{v2.value()->val.str, str.size() + 1u})
  );
}

/**
 * Test that single-cell reference construction works as expected.
 */
TEST_F(Oper12Test, MakeSingleCellTest)
{
  constexpr int row = 1;
  constexpr int col = 5;
  oa::accel::oper12 val{row, col};
  ASSERT_TRUE(val.type() == oa::accel::xltype::sref) << "val is not an sref";
  // note: count is always 1
  EXPECT_EQ(1, val.value()->val.sref.count);
  // note: need parentheses as otherwise macro gets confused
  EXPECT_EQ((xlref12{row, row, col, col}), val.value()->val.sref.ref);
}

/**
 * Test that single reference construction works as expected.
 */
TEST_F(Oper12Test, MakeSingleRefTest)
{
  xlref12 ref{1, 3, 8, 45};
  oa::accel::oper12 val{ref};
  ASSERT_TRUE(val.type() == oa::accel::xltype::sref) << "val is not an sref";
  // note: count is always 1
  EXPECT_EQ(1, val.value()->val.sref.count);
  EXPECT_EQ(ref, val.value()->val.sref.ref);
}

// TODO: add tests for multi-ref

/**
 * Test that float vector construction works as expected.
 *
 * This delegates to the `matrix_view` ctor for indirect testing.
 */
TEST_F(Oper12Test, MakeFloatVectorTest)
{
  std::vector<float> vec{0.f, 1.f, 1.f, 2.f, 3.f, 5.f, 8.f, 13.f, 21.f};
  oa::accel::oper12 val{vec};
  ASSERT_TRUE(val.type() == oa::accel::xltype::multi) << "val is not a multi";
  // note: dimensions should be (vec.size(), 1)
  EXPECT_EQ(vec.size(), val.rows());
  EXPECT_EQ(1, val.cols());
  // get vector for round-trip
  // TODO: really need conversion functions
  std::vector<float> out(val.size());
  for (auto i = 0u; i < out.size(); i++)
    out[i] = static_cast<float>(val[i].val.num);
  // compare values
  EXPECT_THAT(out, ::testing::Pointwise(::testing::FloatEq(), vec));
}

/**
 * Test that double vector construction works as expected.
 *
 * This delegates to the `matrix_view` ctor for indirect testing.
 */
TEST_F(Oper12Test, MakeDoubleVectorTest)
{
  std::vector<double> vec{2., 4., 8., 16., 32., 64., 128., 256., 512.};
  oa::accel::oper12 val{vec};
  ASSERT_TRUE(val.type() == oa::accel::xltype::multi) << "val is not a multi";
  // note: dimensions should be (vec.size(), 1)
  EXPECT_EQ(vec.size(), val.rows());
  EXPECT_EQ(1, val.cols());
  // get vector for round-trip
  std::vector<double> out(val.size());
  for (auto i = 0; i < out.size(); i++)
    out[i] = val[i].val.num;
  // compare values
  EXPECT_THAT(out, ::testing::Pointwise(::testing::DoubleEq(), vec));
}

/**
 * Test that char buffer construction works as expected.
 *
 * This directly tests the `unsigned char` buffer ctor due to delegation.
 */
TEST_F(Oper12Test, MakeCharDataTest)
{
  constexpr const char data[] = {"arbitrary character data"};
  oa::accel::oper12 val{data, sizeof data - 1u};
  ASSERT_TRUE(val.type() == oa::accel::xltype::bigdata) << "val is not bigdata";
  // note: cast to satisfy template type deduction
  EXPECT_EQ(reinterpret_cast<const BYTE*>(data), val.value()->val.bigdata.h.lpbData);
  // note: cast to silence compiler warning + exclude null terminator
  EXPECT_EQ(static_cast<long>(sizeof data - 1u), val.value()->val.bigdata.cbData);
}

/**
 * Test that a nil `oper12` works as expected.
 */
TEST_F(Oper12Test, MakeNilTest)
{
  auto op = oa::accel::oper12::nil();
  EXPECT_EQ(oa::accel::xltype::nil, op.type());
  EXPECT_EQ("[owning=false] nil", oa::accel::to_string(op));
}

/**
 * Test that a missing `oper12` works as expected.
 */
TEST_F(Oper12Test, MakeMissingTest)
{
  auto op = oa::accel::oper12::missing();
  EXPECT_EQ(oa::accel::xltype::missing, op.type());
  EXPECT_EQ("[owning=false] missing", oa::accel::to_string(op));
}

/**
 * Test that `operator!` works as expected.
 */
TEST_F(Oper12Test, NullCheckTest)
{
  oa::accel::oper12 op;
  EXPECT_FALSE(!op);     // contains value
}

}  // namespace
