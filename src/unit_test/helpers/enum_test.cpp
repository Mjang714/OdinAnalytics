#include "gtest/gtest.h"
#include "helpers/utils.h"

namespace
{
	class EnumUtilsTest : public ::testing::Test
	{
		protected:
			void SetUp() override
			{
			
			}

			void TearDown() override
			{

			}

			enum class SpongebobCast
			{
				kSpongebob = 0,
				kSmittyWebermenjenson = 1,
				kSandy = 90,
				kPatrick = 2,
				kPlankton, 
				kKaren = 'k'

			};
	};


	TEST_F(EnumUtilsTest, CheckEnumToIntegers)
	{ 
		
		//Check to to see that Smitty comes in number one cause he was number one
		EXPECT_EQ(1, oa::utils::EnumToInt(SpongebobCast::kSmittyWebermenjenson));

		//check to see if Sandy gives us 90
		EXPECT_EQ(90, oa::utils::EnumToInt(SpongebobCast::kSandy));

		//check to see if Plankton is 3
		EXPECT_EQ(3, oa::utils::EnumToInt(SpongebobCast::kPlankton));

	}

	TEST_F(EnumUtilsTest, ValueToEnum)
	{
		//Test if we feed 1 into the IntToEnum we get Smitty Weberemenjenson cause HE WAS number one! 
		EXPECT_EQ(SpongebobCast::kSmittyWebermenjenson, oa::utils::ValueToEnum<SpongebobCast>(1));

		//Test to see if 90 does indeed give us Sandy
		EXPECT_EQ(SpongebobCast::kSandy, oa::utils::ValueToEnum<SpongebobCast>(90));

		//Test to see if k is given we get kKaren
		EXPECT_EQ(SpongebobCast::kKaren, oa::utils::ValueToEnum<SpongebobCast>('k'));

		//Test to see if we can use the string name of enum to get the enum class value 
		EXPECT_EQ(SpongebobCast::kSmittyWebermenjenson, oa::utils::ValueToEnum<SpongebobCast>("kSmittyWebermenjenson"));
		
	}

	TEST_F(EnumUtilsTest, ValueToEnumInValid)
	{
		//Test if we feed bad a value we get an exception 
		EXPECT_ANY_THROW(oa::utils::ValueToEnum<SpongebobCast>(5));

	}

	TEST_F(EnumUtilsTest, GetEnumName)
	{
		//check to see if we get the string name of the enum
		EXPECT_EQ("kKaren", oa::utils::GetEnumName(SpongebobCast::kKaren));
		EXPECT_EQ("kSmittyWebermenjenson", oa::utils::GetEnumName(SpongebobCast::kSmittyWebermenjenson));
	}

	TEST_F(EnumUtilsTest, GetCleanName)
	{
		//check to see if we get the string name of the enum
		EXPECT_EQ("Karen", oa::utils::GetCleanName(SpongebobCast::kKaren));
		EXPECT_EQ("SmittyWebermenjenson", oa::utils::GetCleanName(SpongebobCast::kSmittyWebermenjenson));
	}

}