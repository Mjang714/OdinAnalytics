#include "gtest/gtest.h"
#include "time/tenor.h"

TEST(TenorClassTest, ValidConstructors)
{
	EXPECT_NO_THROW(oa::time::Tenor("100M"));
	EXPECT_NO_THROW(oa::time::Tenor("100d"));
	EXPECT_NO_THROW(oa::time::Tenor("12341234120Y"));
	EXPECT_NO_THROW(oa::time::Tenor("100y"));
}

TEST(TenorClassTest, InvalidConstructors)
{
	EXPECT_ANY_THROW(oa::time::Tenor("1OM"));
	EXPECT_ANY_THROW(oa::time::Tenor("M10M"));
	EXPECT_ANY_THROW(oa::time::Tenor("20983409M241092384901823M"));
	EXPECT_ANY_THROW(oa::time::Tenor("1843209841903MM"));
	EXPECT_ANY_THROW(oa::time::Tenor("M12341234M"));
	EXPECT_ANY_THROW(oa::time::Tenor("12341234Y2310938490128y"));
}

TEST(TenorClassTest, TenorGetValues)
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

TEST(TenorClassTest, FlipSignFunc)
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