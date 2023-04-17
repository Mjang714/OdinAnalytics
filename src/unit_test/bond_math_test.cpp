#include "gtest/gtest.h"
#include "../BondMathLib/BondMath.h"

namespace
{
	TEST(BondMathTest, YieldWithNumber) {
		EXPECT_GT(getYield(.5, 0, 99.5, 1), .01);

	}

	TEST(BondMathTest, YieldWithString) {
		EXPECT_GT(getYield(.5, 0, 99.5, "pa"), .01);

	}
}