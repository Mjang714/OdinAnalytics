#include "gtest/gtest.h"
#include "helpers/utils.h"

namespace
{
	TEST(Helper_Test, CheckValidTenorStr)
	{
		//Test days
		EXPECT_TRUE(oa::utils::CheckTenorStr("1d"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("23412341234123d"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("1D"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("3285483907510934710975D"));

		//Test weeks
		EXPECT_TRUE(oa::utils::CheckTenorStr("4w"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("198309280w"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("114W"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("1W"));

		//Test Months
		EXPECT_TRUE(oa::utils::CheckTenorStr("6m"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("62384091283049812m"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("12389408120938M"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("18M"));

		//Test years
		EXPECT_TRUE(oa::utils::CheckTenorStr("16y"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("62384091283049812y"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("12389408120938Y"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("1Y"));

		//Test negative tenors
		EXPECT_TRUE(oa::utils::CheckTenorStr("-62384091283049812y"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("-12389408120938Y"));
		EXPECT_TRUE(oa::utils::CheckTenorStr("-1Y"));
	}

	TEST(Helper_Test, CheckInvalidTenorStr)
	{
		//Test days
		EXPECT_FALSE(oa::utils::CheckTenorStr("Od"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("2341234d1234123d"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("D1D"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("D3285483907510934710975D"));

		//Test weeks
		EXPECT_FALSE(oa::utils::CheckTenorStr("4w4"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("1983r09280w"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("1Y14W"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("1O1W"));

		//Test Months
		EXPECT_FALSE(oa::utils::CheckTenorStr("6Om"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("62384091as283049812m"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("12389sadf408120938M"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("18OM"));

		//Test years
		EXPECT_FALSE(oa::utils::CheckTenorStr("1o6y"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("6238409asdf1283049812y"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("123894asdf08120938Y"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("1Y66"));
		
		//Test sign 
		EXPECT_FALSE(oa::utils::CheckTenorStr("62384091-283049812m"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("+12389408120938Y"));
		EXPECT_FALSE(oa::utils::CheckTenorStr("=18OM"));
	}

	TEST(Helper_Test, ValidDateCheck)
	{
		//Test days
		EXPECT_TRUE(oa::utils::CheckDateStr("2022-07-23"));
		EXPECT_TRUE(oa::utils::CheckDateStr("1922/11/5"));
		EXPECT_TRUE(oa::utils::CheckDateStr("2160:9:30"));
		EXPECT_TRUE(oa::utils::CheckDateStr("0009-12-31"));
	}


	TEST(Helper_Test, InvalidDateCheck)
	{

		EXPECT_FALSE(oa::utils::CheckDateStr("22-07-23"));
		EXPECT_FALSE(oa::utils::CheckDateStr("1922/20/05"));
		EXPECT_FALSE(oa::utils::CheckDateStr("2160:9:50"));
		EXPECT_FALSE(oa::utils::CheckDateStr("9-12-04"));
	}
}