#include "gtest/gtest.h"
#include "time/date.h" 

namespace
{
	TEST(DateClassTest, InvalidContructor)
	{
		EXPECT_ANY_THROW(oa::time::Date("190O-12-04"));
		EXPECT_ANY_THROW(oa::time::Date("19-10-2004"));
		EXPECT_ANY_THROW(oa::time::Date("12-10-2007"));
		EXPECT_ANY_THROW(oa::time::Date("12-10-2OO7"));
	}

	TEST(DateClassTest, ValidContructor)
	{
		EXPECT_NO_THROW(oa::time::Date("1900-12-04"));
		EXPECT_NO_THROW(oa::time::Date("1900:12:04"));
		EXPECT_NO_THROW(oa::time::Date("1900/12/04"));

	}

	TEST(DateClassTest, DateStrConstructor)
	{

		oa::time::Date test_date_1("2022-7-31");
		EXPECT_EQ(2459792, test_date_1.GetJulian());
		EXPECT_EQ(6, test_date_1.GetDOWInt());

		//test another date
		oa::time::Date test_date_2("2000-1-1");
		EXPECT_EQ(2451545, test_date_2.GetJulian());
		EXPECT_EQ(5, test_date_2.GetDOWInt());
	}

	TEST(DateClassTest, DateIntConstructor)
	{

		oa::time::Date test_date_1(2459790);
		EXPECT_EQ("2022-7-29 : Julian Integer = 2459790", test_date_1.ToString());
		EXPECT_EQ(4, test_date_1.GetDOWInt());


		oa::time::Date test_date_2(2451556);
		EXPECT_EQ("2000-1-12 : Julian Integer = 2451556", test_date_2.ToString());
		EXPECT_EQ(2, test_date_2.GetDOWInt());
	}

	TEST(DateClassTest, DateTimePointConstructor)
	{
		//Test the tinme_point epoch gives us 1970-1-1
		oa::time::Date epoch_date_expected = oa::time::Date(1970, 1, 1);

		std::chrono::system_clock::time_point tp_epoch;
		oa::time::Date tp_epoch_result = oa::time::Date(tp_epoch);

		EXPECT_TRUE(epoch_date_expected == tp_epoch_result);

	}

	//TODO: finish figuring out how to return timepoint 
	//TEST(DateClassTest, ToTimePointTest)
	//{
	//	oa::Date expected_date = oa::Date(1970, 1, 1);
	//	std::chrono::system_clock::time_point tp_epoch = std::chrono::system_clock::time_point() + ;
 //		std::chrono::system_clock::time_point tp_epoch_result = expected_date.ConvertToTimePt();

	//	EXPECT_EQ(tp_epoch, tp_epoch_result);

	//}

	TEST(DateClassTest, IsLeap)
	{
		// test to see if the year 2000 is a leap year 
		oa::time::Date test_date_1("2000-7-31");
		EXPECT_TRUE(test_date_1.IsLeap());

		// test to see if the year 2024 is a leap year 
		oa::time::Date test_date_2("2024-7-31");
		EXPECT_TRUE(test_date_2.IsLeap());

		// test if the 2100 is not a leap year
		oa::time::Date test_date_3("2100-1-1");
		EXPECT_FALSE(test_date_3.IsLeap());
	}

	TEST(DateClassTest, DaysInMonth)
	{
		//Test to see if we get leap year correct days
		EXPECT_EQ(29, oa::time::Date::DaysInMonth(2, 2000));
		EXPECT_EQ(28, oa::time::Date::DaysInMonth(2, 2100));
		EXPECT_EQ(28, oa::time::Date::DaysInMonth(2, 2030));
		EXPECT_EQ(30, oa::time::Date::DaysInMonth(11, 2010));
		EXPECT_EQ(31, oa::time::Date::DaysInMonth(12, 2020));
	}

	TEST(DateClassTest, DateOpOverloadingRelational)
	{
		// test to see if the year 2000 is a leap year 
		oa::time::Date test_date_1("2000-7-31");
		oa::time::Date test_date_2("2010-10-25");
		oa::time::Date test_date_3("2000-7-31");

		//test false date logic 
		EXPECT_FALSE(test_date_1 == test_date_2);
		EXPECT_FALSE(test_date_1 >= test_date_2);
		EXPECT_FALSE(test_date_1 > test_date_2);
		EXPECT_FALSE(test_date_1 > test_date_3);
		EXPECT_FALSE(test_date_1 < test_date_3);

		//test true logic
		EXPECT_TRUE(test_date_1 == test_date_3);
		EXPECT_TRUE(test_date_1 <= test_date_2);
		EXPECT_TRUE(test_date_1 < test_date_2);

	}

	TEST(DateClassTest, AddDays)
	{
		oa::time::Date base_date(2022,7,29);
		oa::time::Tenor test_tenor_1("2D");
		oa::time::Tenor test_tenor_2("580d");
		oa::time::Tenor test_tenor_3("0d");

		oa::time::Date new_date_1 = base_date.AddTenor(test_tenor_1);
		EXPECT_EQ("2022-7-31 : Julian Integer = 2459792", new_date_1.ToString());

		oa::time::Date new_date_2 = base_date.AddTenor(test_tenor_2);
		EXPECT_EQ("2024-2-29 : Julian Integer = 2460370", new_date_2.ToString());

		oa::time::Date new_date_3 = base_date.AddTenor(test_tenor_3);
		EXPECT_EQ("2022-7-29 : Julian Integer = 2459790", new_date_3.ToString());
	}

	TEST(DateClassTest, AddWeeks)
	{
		oa::time::Date base_date(2022, 7, 28);
		oa::time::Tenor test_tenor_1("2w");
		oa::time::Tenor test_tenor_2("83w");
		oa::time::Tenor test_tenor_3("0w");

		oa::time::Date new_date_1 = base_date.AddTenor(test_tenor_1);
		EXPECT_EQ("2022-8-11 : Julian Integer = 2459803", new_date_1.ToString());

		oa::time::Date new_date_2 = base_date.AddTenor(test_tenor_2);
		EXPECT_EQ("2024-2-29 : Julian Integer = 2460370", new_date_2.ToString());

		oa::time::Date new_date_3 = base_date.AddTenor(test_tenor_3);
		EXPECT_EQ("2022-7-28 : Julian Integer = 2459789", new_date_3.ToString());
	}

	TEST(DateClassTest, AddMonths)
   	{
		oa::time::Date base_date_1(2022, 7, 29);
		oa::time::Tenor test_tenor_1("2M");
		oa::time::Tenor test_tenor_2("19M");
		oa::time::Tenor test_tenor_3("7M");
		oa::time::Tenor test_tenor_4("67M");

		oa::time::Date new_date_1 = base_date_1.AddTenor(test_tenor_1);
		EXPECT_EQ("2022-9-29 : Julian Integer = 2459852", new_date_1.ToString());

		oa::time::Date new_date_2 = base_date_1.AddTenor(test_tenor_2);
		EXPECT_EQ("2024-2-29 : Julian Integer = 2460370", new_date_2.ToString());

		oa::time::Date new_date_3 = base_date_1.AddTenor(test_tenor_3);
		EXPECT_EQ("2023-2-28 : Julian Integer = 2460004", new_date_3.ToString());

		oa::time::Date new_date_4 = base_date_1.AddTenor(test_tenor_4);
		EXPECT_EQ("2028-2-29 : Julian Integer = 2461831", new_date_4.ToString());

		//Test adding month to end of a month wher the last day 31st 
		oa::time::Date base_date_2(2022, 8, 31);
		oa::time::Tenor test_tenor_5("1M");

		oa::time::Date new_date_5 = base_date_2.AddTenor(test_tenor_5);
		EXPECT_EQ("2022-9-30 : Julian Integer = 2459853", new_date_5.ToString());
		
	}

	TEST(DateClassTest, AddYears)
	{
		oa::time::Date base_date_1(2024, 2, 29);
		oa::time::Date base_date_2(2021, 9, 15);
		oa::time::Date base_date_3(2015, 8, 31);

		oa::time::Tenor test_tenor_1("1Y");
		oa::time::Tenor test_tenor_2("60Y");
		oa::time::Tenor test_tenor_3("100Y");

		EXPECT_EQ("2025-2-28 : Julian Integer = 2460735", base_date_1.AddTenor(test_tenor_1).ToString());
		EXPECT_EQ("2081-9-15 : Julian Integer = 2481388", base_date_2.AddTenor(test_tenor_2).ToString());
		EXPECT_EQ("2115-8-31 : Julian Integer = 2493790", base_date_3.AddTenor(test_tenor_3).ToString());
	}

	TEST(DateClassTest, AddNegDays)
	{
		oa::time::Date base_date(2022, 7, 29);
		oa::time::Tenor test_tenor_1("-2D");
		oa::time::Tenor test_tenor_2("-580d");
		oa::time::Tenor test_tenor_3("-0d");

		oa::time::Date new_date_1 = base_date.AddTenor(test_tenor_1);
		EXPECT_EQ("2022-7-27 : Julian Integer = 2459788", new_date_1.ToString());

		oa::time::Date new_date_2 = base_date.AddTenor(test_tenor_2);
		EXPECT_EQ("2020-12-26 : Julian Integer = 2459210", new_date_2.ToString());

		oa::time::Date new_date_3 = base_date.AddTenor(test_tenor_3);
		EXPECT_EQ("2022-7-29 : Julian Integer = 2459790", new_date_3.ToString());
	}

	TEST(DateClassTest, AddNegWeeks)
	{
		oa::time::Date base_date(2022, 7, 28);
		oa::time::Tenor test_tenor_1("-2w");
		oa::time::Tenor test_tenor_2("-83w");
		oa::time::Tenor test_tenor_3("-0w");

		oa::time::Date new_date_1 = base_date.AddTenor(test_tenor_1);
		EXPECT_EQ("2022-7-14 : Julian Integer = 2459775", new_date_1.ToString());

		oa::time::Date new_date_2 = base_date.AddTenor(test_tenor_2);
		EXPECT_EQ("2020-12-24 : Julian Integer = 2459208", new_date_2.ToString());

		oa::time::Date new_date_3 = base_date.AddTenor(test_tenor_3);
		EXPECT_EQ("2022-7-28 : Julian Integer = 2459789", new_date_3.ToString());
	}

	TEST(DateClassTest, AddNegMonths)
	{
		oa::time::Date base_date_1(2022, 7, 29);
		oa::time::Tenor test_tenor_1("-2M");
		oa::time::Tenor test_tenor_2("-29M");
		oa::time::Tenor test_tenor_3("-7M");
		oa::time::Tenor test_tenor_4("-67M");

		oa::time::Date new_date_1 = base_date_1.AddTenor(test_tenor_1);
		EXPECT_EQ("2022-5-29 : Julian Integer = 2459729", new_date_1.ToString());

		oa::time::Date new_date_2 = base_date_1.AddTenor(test_tenor_2);
		EXPECT_EQ("2020-2-29 : Julian Integer = 2458909", new_date_2.ToString());

		oa::time::Date new_date_3 = base_date_1.AddTenor(test_tenor_3);
		EXPECT_EQ("2021-12-29 : Julian Integer = 2459578", new_date_3.ToString());

		oa::time::Date new_date_4 = base_date_1.AddTenor(test_tenor_4);
		EXPECT_EQ("2016-12-29 : Julian Integer = 2457752", new_date_4.ToString());

		////Test adding month to end of a month wher the last day 31st 
		oa::time::Date base_date_2(2022, 8, 31);
		oa::time::Tenor test_tenor_5("-2M");

		oa::time::Date new_date_5 = base_date_2.AddTenor(test_tenor_5);
		EXPECT_EQ("2022-6-30 : Julian Integer = 2459761", new_date_5.ToString());

		// subtract out years 
		oa::time::Tenor test_tenor_6("-36M");

		oa::time::Date new_date_6 = base_date_2.AddTenor(test_tenor_6);
		EXPECT_EQ("2019-8-31 : Julian Integer = 2458727", new_date_6.ToString());
	}

	TEST(DateClassTest, AddNegYears)
	{
		oa::time::Date base_date_1(2024, 2, 29);
		oa::time::Date base_date_2(2081, 9, 15);
		oa::time::Date base_date_3(2015, 8, 31);

		oa::time::Tenor test_tenor_1("-1Y");
		oa::time::Tenor test_tenor_2("-60Y");
		oa::time::Tenor test_tenor_3("-100Y");

		EXPECT_EQ("2023-2-28 : Julian Integer = 2460004", base_date_1.AddTenor(test_tenor_1).ToString());
		EXPECT_EQ("2021-9-15 : Julian Integer = 2459473", base_date_2.AddTenor(test_tenor_2).ToString());
		EXPECT_EQ("1915-8-31 : Julian Integer = 2420741", base_date_3.AddTenor(test_tenor_3).ToString());
	}

	TEST(DateClassTest, SubDays)
	{
		oa::time::Date base_date(2022, 7, 29);
		oa::time::Tenor test_tenor_1("2D");
		oa::time::Tenor test_tenor_2("580d");
		oa::time::Tenor test_tenor_3("0d");

		oa::time::Date new_date_1 = base_date.SubTenor(test_tenor_1);
		EXPECT_EQ("2022-7-27 : Julian Integer = 2459788", new_date_1.ToString());

		oa::time::Date new_date_2 = base_date.SubTenor(test_tenor_2);
		EXPECT_EQ("2020-12-26 : Julian Integer = 2459210", new_date_2.ToString());

		oa::time::Date new_date_3 = base_date.SubTenor(test_tenor_3);
		EXPECT_EQ("2022-7-29 : Julian Integer = 2459790", new_date_3.ToString());
	}

	TEST(DateClassTest, SubWeeks)
	{
		oa::time::Date base_date(2022, 7, 28);
		oa::time::Tenor test_tenor_1("2w");
		oa::time::Tenor test_tenor_2("83w");
		oa::time::Tenor test_tenor_3("0w");

		oa::time::Date new_date_1 = base_date.SubTenor(test_tenor_1);
		EXPECT_EQ("2022-7-14 : Julian Integer = 2459775", new_date_1.ToString());

		oa::time::Date new_date_2 = base_date.SubTenor(test_tenor_2);
		EXPECT_EQ("2020-12-24 : Julian Integer = 2459208", new_date_2.ToString());

		oa::time::Date new_date_3 = base_date.SubTenor(test_tenor_3);
		EXPECT_EQ("2022-7-28 : Julian Integer = 2459789", new_date_3.ToString());
	}

	TEST(DateClassTest, SubMonths)
	{
		oa::time::Date base_date_1(2022, 7, 29);
		oa::time::Tenor test_tenor_1("2M");
		oa::time::Tenor test_tenor_2("29M");
		oa::time::Tenor test_tenor_3("7M");
		oa::time::Tenor test_tenor_4("67M");

		oa::time::Date new_date_1 = base_date_1.SubTenor(test_tenor_1);
		EXPECT_EQ("2022-5-29 : Julian Integer = 2459729", new_date_1.ToString());

		oa::time::Date new_date_2 = base_date_1.SubTenor(test_tenor_2);
		EXPECT_EQ("2020-2-29 : Julian Integer = 2458909", new_date_2.ToString());

		oa::time::Date new_date_3 = base_date_1.SubTenor(test_tenor_3);
		EXPECT_EQ("2021-12-29 : Julian Integer = 2459578", new_date_3.ToString());

		oa::time::Date new_date_4 = base_date_1.SubTenor(test_tenor_4);
		EXPECT_EQ("2016-12-29 : Julian Integer = 2457752", new_date_4.ToString());

		////Test adding month to end of a month wher the last day 31st 
		oa::time::Date base_date_2(2022, 8, 31);
		oa::time::Tenor test_tenor_5("2M");

		oa::time::Date new_date_5 = base_date_2.SubTenor(test_tenor_5);
		EXPECT_EQ("2022-6-30 : Julian Integer = 2459761", new_date_5.ToString());

		// subtract out years 
		oa::time::Tenor test_tenor_6("36M");

		oa::time::Date new_date_6 = base_date_2.SubTenor(test_tenor_6);
		EXPECT_EQ("2019-8-31 : Julian Integer = 2458727", new_date_6.ToString());
	}

	TEST(DateClassTest, SubYears)
	{
		oa::time::Date base_date_1(2024, 2, 29);
		oa::time::Date base_date_2(2081, 9, 15);
		oa::time::Date base_date_3(2015, 8, 31);

		oa::time::Tenor test_tenor_1("1Y");
		oa::time::Tenor test_tenor_2("60Y");
		oa::time::Tenor test_tenor_3("100Y");

		EXPECT_EQ("2023-2-28 : Julian Integer = 2460004", base_date_1.SubTenor(test_tenor_1).ToString());
		EXPECT_EQ("2021-9-15 : Julian Integer = 2459473", base_date_2.SubTenor(test_tenor_2).ToString());
		EXPECT_EQ("1915-8-31 : Julian Integer = 2420741", base_date_3.SubTenor(test_tenor_3).ToString()); 
	}

	TEST(DateClassTest, SubNegDays)
	{
		oa::time::Date base_date(2022, 7, 29);
		oa::time::Tenor test_tenor_1("-2D");
		oa::time::Tenor test_tenor_2("-580d");
		oa::time::Tenor test_tenor_3("-0d");

		oa::time::Date new_date_1 = base_date.SubTenor(test_tenor_1);
		EXPECT_EQ("2022-7-31 : Julian Integer = 2459792", new_date_1.ToString());

		oa::time::Date new_date_2 = base_date.SubTenor(test_tenor_2);
		EXPECT_EQ("2024-2-29 : Julian Integer = 2460370", new_date_2.ToString());

		oa::time::Date new_date_3 = base_date.SubTenor(test_tenor_3);
		EXPECT_EQ("2022-7-29 : Julian Integer = 2459790", new_date_3.ToString());
	}

	TEST(DateClassTest, SubNegWeeks)
	{
		oa::time::Date base_date(2022, 7, 28);
		oa::time::Tenor test_tenor_1("-2w");
		oa::time::Tenor test_tenor_2("-83w");
		oa::time::Tenor test_tenor_3("-0w");

		oa::time::Date new_date_1 = base_date.SubTenor(test_tenor_1);
		EXPECT_EQ("2022-8-11 : Julian Integer = 2459803", new_date_1.ToString());

		oa::time::Date new_date_2 = base_date.SubTenor(test_tenor_2);
		EXPECT_EQ("2024-2-29 : Julian Integer = 2460370", new_date_2.ToString());

		oa::time::Date new_date_3 = base_date.SubTenor(test_tenor_3);
		EXPECT_EQ("2022-7-28 : Julian Integer = 2459789", new_date_3.ToString());
	}

	TEST(DateClassTest, SubNegMonths)
	{
		oa::time::Date base_date_1(2022, 7, 29);
		oa::time::Tenor test_tenor_1("-2M");
		oa::time::Tenor test_tenor_2("-19M");
		oa::time::Tenor test_tenor_3("-7M");
		oa::time::Tenor test_tenor_4("-67M");

		oa::time::Date new_date_1 = base_date_1.SubTenor(test_tenor_1);
		EXPECT_EQ("2022-9-29 : Julian Integer = 2459852", new_date_1.ToString());

		oa::time::Date new_date_2 = base_date_1.SubTenor(test_tenor_2);
		EXPECT_EQ("2024-2-29 : Julian Integer = 2460370", new_date_2.ToString());

		oa::time::Date new_date_3 = base_date_1.SubTenor(test_tenor_3);
		EXPECT_EQ("2023-2-28 : Julian Integer = 2460004", new_date_3.ToString());

		oa::time::Date new_date_4 = base_date_1.SubTenor(test_tenor_4);
		EXPECT_EQ("2028-2-29 : Julian Integer = 2461831", new_date_4.ToString());

		//Test adding month to end of a month wher the last day 31st 
		oa::time::Date base_date_2(2022, 8, 31);
		oa::time::Tenor test_tenor_5("-1M");

		oa::time::Date new_date_5 = base_date_2.SubTenor(test_tenor_5);
		EXPECT_EQ("2022-9-30 : Julian Integer = 2459853", new_date_5.ToString());

	}

	TEST(DateClassTest, SubNegYears)
	{
		oa::time::Date base_date_1(2024, 2, 29);
		oa::time::Date base_date_2(2021, 9, 15);
		oa::time::Date base_date_3(2015, 8, 31);

		oa::time::Tenor test_tenor_1("-1Y");
		oa::time::Tenor test_tenor_2("-60Y");
		oa::time::Tenor test_tenor_3("-100Y");

		EXPECT_EQ("2025-2-28 : Julian Integer = 2460735", base_date_1.SubTenor(test_tenor_1).ToString());
		EXPECT_EQ("2081-9-15 : Julian Integer = 2481388", base_date_2.SubTenor(test_tenor_2).ToString());
		EXPECT_EQ("2115-8-31 : Julian Integer = 2493790", base_date_3.SubTenor(test_tenor_3).ToString());
	}


}