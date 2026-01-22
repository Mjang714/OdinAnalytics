/**
 * @file xl_conv_test.cpp
 * @author Derek Huang
 * @brief xl_conv.h unit tests
 * @copyright MIT License
 */

#include "oa/accel/xl_conv.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif  // WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <XLCALL.H>

#include <memory>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

/**
 * `xl_conv.h` unit test fixture.
 */
class XlConvTest : public ::testing::Test {};

/**
 * Test converting `xltypeNum` to a double.
 */
TEST_F(XlConvTest, DoubleTest)
{
  constexpr auto val = 3.14159;
  xloper12 op{};
  op.val.num = val;
  op.xltype = xltypeNum;
  EXPECT_DOUBLE_EQ(val, oa::accel::as<double>(op));
}

/**
 * Test converting `xltypeNum` to a float.
 */
TEST_F(XlConvTest, FloatTest)
{
  constexpr auto val = 2.71f;
  xloper12 op{};
  op.val.num = val;
  op.xltype = xltypeNum;
  EXPECT_FLOAT_EQ(val, oa::accel::as<float>(op));
}

/**
 * Test converting `xltypeInt` to a float.
 */
TEST_F(XlConvTest, Int2FloatTest)
{
  constexpr int val = 8888;
  xloper12 op{};
  op.val.w = val;
  op.xltype = xltypeInt;
  EXPECT_FLOAT_EQ(val, oa::accel::as<float>(op));
}

/**
 * Test converting `xltypeBool` to a float.
 */
TEST_F(XlConvTest, Bool2FloatTest)
{
  xloper12 op{};
  op.val.xbool = 1;
  op.xltype = xltypeBool;
  EXPECT_FLOAT_EQ(1.f, oa::accel::as<float>(op));
}

/**
 * Test converting `xltypeInt` to an int.
 */
TEST_F(XlConvTest, IntTest)
{
  constexpr int val = 123;
  xloper12 op{};
  op.val.w = val;
  op.xltype = xltypeInt;
  EXPECT_EQ(val, oa::accel::as<int>(op));
}

/**
 * Test converting from `xltypeBool` to an unsigned int.
 */
TEST_F(XlConvTest, Bool2UIntTest)
{
  xloper12 op{};
  op.val.xbool = 1;
  op.xltype = xltypeBool;
  EXPECT_EQ(1u, oa::accel::as<unsigned>(op));
}

/**
 * Test converting from `xltypeInt` to a `bool`.
 */
TEST_F(XlConvTest, Int2BoolTest)
{
  xloper12 op{};
  op.val.w = 123;
  op.xltype = xltypeInt;
  EXPECT_TRUE(oa::accel::as<bool>(op));
}

// TODO: add conversion tests for strings

/**
 * Test converting to a vector of doubles.
 *
 * This test demonstrates how dimensions are flattened on conversion.
 */
TEST_F(XlConvTest, DoubleVectorTest)
{
  constexpr double val[] = {0., 1., 1., 2., 3., 5., 8., 13., 21., 34.};
  // allocate buffer for new xloper12 + fill
  auto buf = std::make_unique<xloper12[]>(std::size(val));
  for (auto i = 0u; i < std::size(val); i++) {
    buf[i].val.num = val[i];
    buf[i].xltype = xltypeNum;
  }
  // create xltypeMulti + fill
  xloper12 op{};
  op.val.array.lparray = buf.get();
  // note: act as if we want a real 2D array
  op.val.array.rows = 2;
  op.val.array.columns = 5;
  op.xltype = xltypeMulti;
  // test conversion
  auto res = oa::accel::as<std::vector<double>>(op);
  EXPECT_THAT(res, ::testing::Pointwise(::testing::DoubleEq(), val));
}

/**
 * Test converting to a column vector of doubles.
 *
 * This test demonstrates the use of `multi_conv_options` in conversion.
 */
TEST_F(XlConvTest, DoubleColVectorTest)
{
  constexpr double val[] = {1., 2., 4., 8., 16., 32., 64., 128.};
  // allocate buffer for new xloper12 + fill
  auto buf = std::make_unique<xloper12[]>(std::size(val));
  for (auto i = 0u; i < std::size(val); i++) {
    buf[i].val.num = val[i];
    buf[i].xltype = xltypeNum;
  }
  // create xltypeMulti + fill
  xloper12 op{};
  op.val.array.lparray = buf.get();
  // purposefully set dimensions incorrectly to trigger exception
  op.val.array.rows = 1;
  op.val.array.columns = static_cast<RW>(std::size(val));
  op.xltype = xltypeMulti;
  // create options + check that vector() is also set
  auto opts = oa::accel::multi_conv_options{}.col_vector(true);
  EXPECT_EQ(opts.col_vector(), opts.vector());
  // test bad conversion
  EXPECT_ANY_THROW(oa::accel::as<std::vector<double>>(op, opts));
  // fix dimensions + test conversion
  op.val.array.rows = static_cast<RW>(std::size(val));
  op.val.array.columns = 1;
  auto res = oa::accel::as<std::vector<double>>(op, opts);
  EXPECT_THAT(res, ::testing::Pointwise(::testing::DoubleEq(), val));
}

/**
 * Test converting an int into a float vector of size 1.
 */
TEST_F(XlConvTest, Int2FloatVectorTest)
{
  constexpr int val[] = {1234};
  // allocate buffer for new xloper12 + fill
  auto buf = std::make_unique<xloper12[]>(1u);
  buf[0].val.num = val[0];
  buf[0].xltype = xltypeNum;
  // create xltypeMulti + fill
  xloper12 op{};
  op.val.array.lparray = buf.get();
  op.val.array.rows = op.val.array.columns = 1;
  op.xltype = xltypeMulti;
  // test conversion
  auto res = oa::accel::as<std::vector<float>>(op);
  EXPECT_THAT(res, ::testing::Pointwise(::testing::FloatEq(), val));
}

}  // namespace
