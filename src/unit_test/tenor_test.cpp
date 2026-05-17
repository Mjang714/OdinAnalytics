#include "time/tenor.h"

#include <cstddef>
#include <set>
#include <unordered_set>
#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "time/time_enums.h"

namespace {

/**
 * Test fixture for the `Tenor` tests.
 */
class TenorTest : public ::testing::Test {
protected:
	// convenience alias for tests
	using Tenor = oa::time::Tenor;
	using Tenors = oa::time::Tenors;
};

TEST_F(TenorTest, ValidConstructors)
{
	EXPECT_NO_THROW(oa::time::Tenor("100M"));
	EXPECT_NO_THROW(oa::time::Tenor("100d"));
	EXPECT_NO_THROW(oa::time::Tenor("1234123412Y"));
	EXPECT_NO_THROW(oa::time::Tenor("100y"));
}

TEST_F(TenorTest, InvalidConstructors)
{
	EXPECT_ANY_THROW(oa::time::Tenor("1OM"));
	EXPECT_ANY_THROW(oa::time::Tenor("M10M"));
	EXPECT_ANY_THROW(oa::time::Tenor("20983409M241092384901823M"));
	EXPECT_ANY_THROW(oa::time::Tenor("1843209841903MM"));
	EXPECT_ANY_THROW(oa::time::Tenor("M12341234M"));
	EXPECT_ANY_THROW(oa::time::Tenor("12341234Y2310938490128y"));
}

TEST_F(TenorTest, TenorGetValues)
{
	oa::time::Tenor tenor_1("100M");
	auto first_values = tenor_1.GetValues();

	EXPECT_EQ(100, first_values.first);
	EXPECT_EQ(oa::time::Tenors::kMonths, first_values.second);

	oa::time::Tenor tenor_2("5y");
	auto second_values = tenor_2.GetValues();

	EXPECT_EQ(5, second_values.first);
	EXPECT_EQ(oa::time::Tenors::kYears, second_values.second);

	oa::time::Tenor tenor_3("-10y");
	auto third_values = tenor_3.GetValues();

	EXPECT_EQ(-10, third_values.first);
	EXPECT_EQ(oa::time::Tenors::kYears, third_values.second);

}

TEST_F(TenorTest, FlipSignFunc)
{
	oa::time::Tenor tenor_1("5y");
	oa::time::Tenor tenor_2 = tenor_1.FlipSign();
	auto flipped_val_1 = tenor_2.GetValues();

	EXPECT_EQ(-5, flipped_val_1.first);
	EXPECT_EQ(oa::time::Tenors::kYears, flipped_val_1.second);

	oa::time::Tenor tenor_3 = tenor_2.FlipSign();
	auto flipped_val_2 = tenor_3.GetValues();

	EXPECT_EQ(5, flipped_val_2.first);
	EXPECT_EQ(oa::time::Tenors::kYears, flipped_val_2.second);
}

/**
 * Test that negating the count of a `Tenor` works as expected.
 */
TEST_F(TenorTest, NegationTest)
{
	Tenor t1{5, Tenors::kMonths};
	Tenor t2{-t1.count(), t1.unit()};
	EXPECT_EQ(t2, -t1);
}

/**
 * Test that adding an integer to a `Tenor` works as expected.
 */
TEST_F(TenorTest, LeftAddTest)
{
	Tenor t1{10, Tenors::kWeeks};
	Tenor t2{16, t1.unit()};
	EXPECT_EQ(t2, t1 + 6);
}

/**
 * Test that adding a `Tenor` to an integer works as expected.
 */
TEST_F(TenorTest, RightAddTest)
{
	Tenor t1{5, Tenors::kYears};
	Tenor t2{8, t1.unit()};
	EXPECT_EQ(t2, 3 + t1);
}

/**
 * Test that substracting an integer from a `Tenor` works as expected.
 */
TEST_F(TenorTest, SubtractTest)
{
	Tenor t1{3, Tenors::kMonths};
	Tenor t2{1, t1.unit()};
	EXPECT_EQ(t2, t1 - 2);
}

/**
 * Test that `Tenor::GroupLess` works with `std::set` and orders correctly.
 */
TEST_F(TenorTest, GroupLessSetTest)
{
	// construct from initializer list
	std::set<Tenor, Tenor::GroupLess> tenors{
		{1, Tenors::kYears},
		{2, Tenors::kMonths},
		{3, Tenors::kWeeks},
		{4, Tenors::kDays},
		{1, Tenors::kDays},
		{2, Tenors::kWeeks},
		{3, Tenors::kMonths},
		{4, Tenors::kYears}
	};
	// vector of tenors in the expected order
	std::vector<Tenor> ref_tenors{
		{1, Tenors::kDays},
		{4, Tenors::kDays},
		{2, Tenors::kWeeks},
		{3, Tenors::kWeeks},
		{2, Tenors::kMonths},
		{3, Tenors::kMonths},
		{1, Tenors::kYears},
		{4, Tenors::kYears}
	};
	// compare
	// note: GoogleMock is smart enough to compare STL containers of different
	// type as long as the value_type type members are the same
	EXPECT_THAT(tenors, ::testing::Pointwise(::testing::Eq(), ref_tenors));
}

/**
 * Test that `std::hash<Tenor>` works with `std::unordered_set`.
 */
TEST_F(TenorTest, HashSetTest)
{
	// note: not in any particular order
	std::unordered_set<Tenor> tenors{
		{1, Tenors::kYears},
		{2, Tenors::kMonths},
		{3, Tenors::kWeeks},
		{4, Tenors::kDays},
		{1, Tenors::kDays},
		{2, Tenors::kWeeks},
		{3, Tenors::kMonths},
		{4, Tenors::kYears}
	};
	// should see all elements in the set
	EXPECT_EQ(8u, tenors.size());
	// all hash values should be unique
	// note: one might be tempted to check the buckets in the set to see if
	// each bucket has <=1 element, but elements with different hashes can end
	// up in the same bucket anyways. so it's an unhelpful check
	std::unordered_set<std::size_t> hashes;
	for (const auto& tenor : tenors)
		hashes.insert(std::hash<Tenor>{}(tenor));
	// should be exactly 8 different hash values
	EXPECT_EQ(8u, hashes.size());
}

}  // namespace
