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

#include <cstddef>
#include <string>
#include <tuple>
#include <utility>
#include <variant>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "oa/accel/enums.h"
#include "oa/accel/xl_ops.h"  // for xlref12 operator==

namespace {

/**
 * `oper12` test fixture.
 */
class Oper12Test : public ::testing::Test {
private:
  using oper12 = oa::accel::oper12;

  /**
   * Check that an `oper12` of type `xltypeMulti` converts to the given values.
   *
   * The conversion is a strict conversion for testing purposes.
   *
   * @tparam Is Indices 0 through `sizeof...(Ts)` - 1
   * @tparam Ts Input types
   */
  template <std::size_t... Is, typename... Ts>
  auto check(
    std::index_sequence<Is...>,
    const oper12& op,
    const std::tuple<Ts...>& values) const
  {
    static_assert(sizeof...(Is) == sizeof...(Ts));
    // not xltypeMulti
    if (op.type() != oa::accel::xltype::multi)
      return ::testing::AssertionFailure() << "op.type() is " << op.type() <<
        " != xltypeMulti";
    // assume success for now
    auto res = ::testing::AssertionSuccess();
    // fold over pack to check
    (
      [&op, &values, &res]
      {
        // helper lambda to reset res to AssertionFailure()
        auto mark_fail = [&res]() mutable
        {
          if (res)
            res = ::testing::AssertionFailure();
        };
        // out of bounds
        if (Is >= op.size()) {
          mark_fail();
          res << "\nop(" << Is << ") missing";
          return;
        }
        // attempt conversion (could throw)
        try {
          if (op(Is).as<Ts>(oa::accel::strict) != std::get<Is>(values)) {
            mark_fail();
            res << "\nop(" << Is << ") != values[" << Is << "] [" << op <<
              " != " << std::get<Is>(values) << "]";
          }
        }
        catch (const std::exception& exc) {
          mark_fail();
          res << "\nop(" << Is << ") != values[" << Is << "]: exception: " <<
            exc.what();
        }
      }(), ...
    );
    // return
    return res;
  }

protected:
  /**
   * Check that an `oper12` of type `xltypeMulti` converts to the given values.
   *
   * The conversion is a strict conversion for testing purposes.
   *
   * @tparam Ts Expected input values
   */
  template <typename... Ts>
  auto check(const oper12& op, const std::tuple<Ts...>& values) const
  {
    return check(std::index_sequence_for<Ts...>{}, op, values);
  }
};

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
  ASSERT_EQ(oa::accel::xltype::bool_, val.type());
  // TODO: add conversion for value inspection
  EXPECT_EQ(x, !!val->val.xbool);
}

/**
 * Test that `int` construction works as expected.
 */
TEST_F(Oper12Test, MakeIntTest)
{
  constexpr int x = 100;
  oa::accel::oper12 val{x};
  ASSERT_EQ(oa::accel::xltype::int_, val.type());
  // TODO: add conversion for value inspection
  EXPECT_EQ(x, val->val.w);
}

/**
 * Test that `double` construction works as expected.
 */
TEST_F(Oper12Test, MakeDoubleTest)
{
  constexpr double x = 2.;
  oa::accel::oper12 val{x};
  ASSERT_EQ(oa::accel::xltype::num, val.type());
  // TODO: add conversion for value inspection
  EXPECT_DOUBLE_EQ(x, val->val.num);
}

/**
 * Test that string literal construction works as expected.
 */
TEST_F(Oper12Test, MakeCStringTest)
{
  // note: using character array so we can use begin() and end()
  constexpr const char str[] = "hello world";
  oa::accel::oper12 val{str};
  ASSERT_EQ(oa::accel::xltype::str, val.type());
  // owns extra memory
  EXPECT_TRUE(val.owning());
  // TODO: add conversion for value inspection
  EXPECT_EQ(
    // note: subtract one from end(str) to avoid null terminator
    (L"\013" + std::wstring{std::begin(str), std::end(str) - 1u}),
    // note: use sizeof str because Excel string has prepended size
    (std::wstring_view{val->val.str, sizeof str})
  );
}

/**
 * Test that `std::string` construction works as expected.
 */
TEST_F(Oper12Test, MakeStringTest)
{
  std::string str{"hello world"};
  oa::accel::oper12 val{str};
  ASSERT_EQ(oa::accel::xltype::str, val.type());
  // owns extra memory
  EXPECT_TRUE(val.owning());
  // TODO: add conversion for value inspection
  // Excel 12 uses wide strings and size is in first code point
  EXPECT_EQ(
    (L"\013" + std::wstring{str.begin(), str.end()}),
    (std::wstring_view{val->val.str, str.size() + 1u})
  );
}

/**
 * Test that `std::wstring` construction works as expected.
 */
TEST_F(Oper12Test, MakeWstringTest)
{
  std::wstring str{L"hello world"};
  oa::accel::oper12 val{str};
  ASSERT_EQ(oa::accel::xltype::str, val.type());
  // owns extra memory
  EXPECT_TRUE(val.owning());
  // TODO: add conversion for value inspection
  // Excel 12 wide string size is in first code point
  EXPECT_EQ(L"\013" + str, (std::wstring_view{val->val.str, str.size() + 1u}));
}

/**
 * Test that `std::wstring` copy works as expected.
 */
TEST_F(Oper12Test, CopyWStringTest)
{
  std::wstring str{L"hello world"};
  oa::accel::oper12 v1{str};
  auto v2 = v1;
  ASSERT_EQ(v1.type(), v2.type());
  EXPECT_EQ(v1.owning(), v2.owning());
  // TODO: add conversion for value inspection
  // note: Excel strings have size prepended into first character
  EXPECT_EQ(
    (std::wstring_view{v1->val.str, str.size() + 1u}),
    (std::wstring_view{v2->val.str, str.size() + 1u})
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
  ASSERT_EQ(oa::accel::xltype::str, v2.type());
  // TODO: add conversion for value inspection
  EXPECT_EQ(L"\013" + str, (std::wstring_view{v2->val.str, str.size() + 1u}));
}

/**
 * Test that single-cell reference construction works as expected.
 */
TEST_F(Oper12Test, MakeSingleCellTest)
{
  constexpr int row = 1;
  constexpr int col = 5;
  oa::accel::oper12 val{row, col};
  ASSERT_EQ(oa::accel::xltype::sref, val.type());
  // note: count is always 1
  EXPECT_EQ(1, val->val.sref.count);
  // note: need parentheses as otherwise macro gets confused
  EXPECT_EQ((xlref12{row, row, col, col}), val->val.sref.ref);
}

/**
 * Test that single reference construction works as expected.
 */
TEST_F(Oper12Test, MakeSingleRefTest)
{
  xlref12 ref{1, 3, 8, 45};
  oa::accel::oper12 val{ref};
  ASSERT_EQ(oa::accel::xltype::sref, val.type());
  // note: count is always 1
  EXPECT_EQ(1, val->val.sref.count);
  EXPECT_EQ(ref, val->val.sref.ref);
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
  ASSERT_EQ(oa::accel::xltype::multi, val.type());
  // note: dimensions should be (vec.size(), 1)
  EXPECT_EQ(vec.size(), val.rows());
  EXPECT_EQ(1, val.cols());
  // get vector for round-trip
  // TODO: really need conversion functions
  std::vector<float> out(val.size());
  for (auto i = 0u; i < out.size(); i++)
    out[i] = static_cast<float>(val(i)->val.num);
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
  ASSERT_EQ(oa::accel::xltype::multi, val.type());
  // note: dimensions should be (vec.size(), 1)
  EXPECT_EQ(vec.size(), val.rows());
  EXPECT_EQ(1, val.cols());
  // get vector for round-trip
  std::vector<double> out(val.size());
  for (auto i = 0; i < out.size(); i++)
    out[i] = val(i)->val.num;
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
  ASSERT_EQ(oa::accel::xltype::bigdata, val.type());
  // note: cast to satisfy template type deduction
  EXPECT_EQ(reinterpret_cast<const BYTE*>(data), val->val.bigdata.h.lpbData);
  // note: cast to silence compiler warning + exclude null terminator
  EXPECT_EQ(static_cast<long>(sizeof data - 1u), val->val.bigdata.cbData);
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

/**
 * Test creating an `oper12` from an array of `oper12` values.
 *
 * By implicit conversion to `matrix_view<T>` the resulting `oper12` creates a
 * single-column matrix when returned to Excel.
 */
TEST_F(Oper12Test, Oper12VectorTest)
{
  std::vector<oa::accel::oper12> vec{2., 1, "hello", true, "no way bro"};
  oa::accel::oper12 op{vec};
  // check dimensions and values
  EXPECT_EQ(vec.size(), op.rows());
  EXPECT_EQ(1u, op.cols());
  // note: need exact types due to template deduction rules
  std::tuple vals{2., 1, std::string{"hello"}, true, std::string{"no way bro"}};
  EXPECT_TRUE(check(op, vals));
}

/**
 * Test creating an `oper12` from a 2D array of `oper12` values.
 */
TEST_F(Oper12Test, Oper12MatrixTest)
{
  std::vector<oa::accel::oper12> vec{1., 2, "hello", true, false, "world"};
  constexpr auto n_rows = 3u;
  constexpr auto n_cols = 2u;
  oa::accel::oper12 op{{vec.data(), n_rows, n_cols}};
  // check dimensions and values
  EXPECT_EQ(n_rows, op.rows());
  EXPECT_EQ(n_cols, op.cols());
  // note: need exact types due to template deduction rules
  std::tuple vals{1., 2, std::string{"hello"}, true, false, std::string{"world"}};
  EXPECT_TRUE(check(op, vals));
}

/**
 * Test creating an `oper12` from a nested initializer list.
 */
TEST_F(Oper12Test, InitListTest)
{
  // direct list-initialization
  oa::accel::oper12 op{
    {1., "hello", true, 5},
    {2., "world", false, 10},
    {3., "water", true, 15}
  };
  // check dimensions
  EXPECT_EQ(3u, op.rows());
  EXPECT_EQ(4u, op.cols());
  // flat value comparison should work
  std::tuple vals{
    1., std::string{"hello"}, true, 5,
    2., std::string{"world"}, false, 10,
    3., std::string{"water"}, true, 15
  };
  EXPECT_TRUE(check(op, vals));
}

}  // namespace
