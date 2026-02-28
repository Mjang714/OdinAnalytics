/**
 * @file xl_array_test.cpp
 * @author Derek Huang
 * @brief xl_array.h unit tests
 * @copyright MIT License
 */

#include "oxl/xl_api/xl_array.h"

#include <iostream>
#include <stdexcept>
#include <string>
#include <variant>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace {

/**
 * Test fixture for `XlArray` tests.
 */
class XlArrayTest : public ::testing::Test {
protected:
  // convenience type aliases
  using XlArray = oxl::xl_api::XlArray;
  using XlVariant = oxl::xl_api::XlVariant;

  // starting value for every test
  XlArray value_{
    {1., std::string{"hello"}, true},
    {2., std::string{"cold"}, false},
    {3., std::string{"tidal"}, true},
    {4., std::string{"water"}, false}
  };
};

/**
 * Test that dimensions are initialized correctly.
 */
TEST_F(XlArrayTest, InitDimTest)
{
  EXPECT_EQ(4u, value_.rows());
  EXPECT_EQ(3u, value_.cols());
}

/**
 * Test that setting entire rows works correctly.
 */
TEST_F(XlArrayTest, RowUpdateTest)
{
  // note: purposefully testing setting via initializer list
  value_[0] = {8., std::string{"a"}, true};
  // values should match
  // note: due to template deduction rules no conversion is done. so we either
  // have to use the variant ctor directly, use std::get<T>, or provide our own
  // operator== for XlVariant and one of its type members
  EXPECT_EQ(value_[0][0], XlVariant{8.});
  EXPECT_EQ(value_[0][1], XlVariant{std::string{"a"}});
  EXPECT_EQ(value_[0][2], XlVariant{true});
}

/**
 * Test that setting rows of the incorrect size is caught.
 */
TEST_F(XlArrayTest, BadRowUpdateTest)
{
  EXPECT_THROW((value_[0] = {true, false}), std::invalid_argument);
}

/**
 * Test that initializing a new `XlArray` with ragged rows is caught.
 */
TEST_F(XlArrayTest, BadInitTest)
{
  // note: using lambda since preprocessor thinks extra commas delineate args
  auto bad_array = []() -> XlArray
  {
    return {
      {1., false, std::string{"dog"}},
      {2., true, std::string{"cat"}, std::string{"goat"}}  // extra value
    };
  };
  EXPECT_THROW(bad_array(), std::invalid_argument);
}

/**
 * Test that copied rows are not affected by row update.
 */
TEST_F(XlArrayTest, RowCopyTest)
{
  // copy first row
  auto row_0 = *value_[0];
  // assign using values from second row
  // note: omitting the copy is a logical, but not compiler error!
  value_[0] = *value_[1];
  // first and second rows should be equal
  EXPECT_EQ(value_[0], value_[1]);
  // row_0 should *not* equal value_[1]
  // note: Google Test is smart enough to compare values even if the containers
  // are different types as long as value_type is the same
  EXPECT_THAT(value_[1], ::testing::Pointwise(::testing::Ne(), row_0));
}

/**
 * Test mixed modifiable and non-modifiable row equality.
 */
TEST_F(XlArrayTest, MixedConstRowEqTest)
{
  EXPECT_EQ(value_[0], const_cast<const XlArray&>(value_)[0]);
  // note: operator!= synthesized from operator== in C++20
  EXPECT_NE(static_cast<const XlArray&>(value_)[0], value_[1]);
}

/**
 * Test range-for iteration through a row.
 */
TEST_F(XlArrayTest, RowRangeForTest)
{
  for (auto& v : value_[0])
    static_cast<void>(v);    // tell compiler to ignore
}

}  // namespace
