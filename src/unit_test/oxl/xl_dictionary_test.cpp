/**
 * @file xl_dictionary_test.cpp
 * @author Derek Huang
 * @brief xl_dictionary.h unit test
 * @copyright MIT License
 */

#include "oxl/xl_api/xl_dictionary.h"

#include <array>
#include <ranges>
#include <vector>

#include <gtest/gtest.h>

#include "oxl/xl_api/xl_variant.h"

namespace {

/**
 * Test fixture for `XlDictionary` tests.
 */
class XlDictionaryTest : public ::testing::Test {
protected:
  using XlDictionary = oxl::xl_api::XlDictionary;
  using XlVariant = oxl::xl_api::XlVariant;
};

/**
 * Test constructing from string keys and variant values.
 */
TEST_F(XlDictionaryTest, KeyValueInitTest)
{
  // purposefully different key and value input range types
  std::array<std::string, 3u> keys{"hello", "world", "now"};
  std::vector<XlVariant> values{1., true, "dog"};
  // create dict
  XlDictionary dict{keys, values};
  // check values
  EXPECT_EQ(1., std::get<double>(dict["hello"]));
  EXPECT_EQ(true, std::get<bool>(dict["world"]));
  EXPECT_EQ("dog", std::get<std::string>(dict["now"]));
}

/**
 * Test constructing from variant keys and variant values.
 */
TEST_F(XlDictionaryTest, XlValueInitTest)
{
  // purposefully different key and value input range types
  XlVariant keys[] = {"a", "b", "c"};
  std::vector<XlVariant> values{1., 2., true};
  // create dict
  XlDictionary dict{keys, values};
  // check values
  EXPECT_EQ(1., std::get<double>(dict["a"]));
  EXPECT_EQ(2., std::get<double>(dict["b"]));
  EXPECT_EQ(true, std::get<bool>(dict["c"]));
}

/**
 * Test that the ctor correctly allows only string keys.
 */
TEST_F(XlDictionaryTest, XlValueInitCheckTest)
{
  // use lambda to simplify expression valuation
  auto bad_dict = []
  {
    XlVariant keys[] = {"a", 8888., "b"};
    XlVariant values[] = {1., 2., 3.};
    XlDictionary dict{keys, values};
  };
  EXPECT_THROW(bad_dict(), std::bad_variant_access);
}

/**
 * Test that construction from initializer list works.
 */
TEST_F(XlDictionaryTest, InitListTest)
{
  XlDictionary dict{{"a", 1.}, {"b", "world"}, {"c", false}, {"d", true}};
  // check values
  EXPECT_EQ(1., std::get<double>(dict["a"]));
  EXPECT_EQ("world", std::get<std::string>(dict["b"]));
  EXPECT_EQ(false, std::get<bool>(dict["c"]));
  EXPECT_EQ(true, std::get<bool>(dict["d"]));
}

/**
 * Test that the `XlDictionary` supports range-for.
 */
TEST_F(XlDictionaryTest, RangeForTest)
{
  XlDictionary dict{{"a", 3.}, {"b", false}, {"c", "true"}};
  // modify value (key is not modifiable)
  for (auto& [key, value] : dict)
    value = true;
  // check values
  EXPECT_TRUE(std::get<bool>(dict["a"]));
  EXPECT_TRUE(std::get<bool>(dict["b"]));
  EXPECT_TRUE(std::get<bool>(dict["c"]));
}

}  // namespace
