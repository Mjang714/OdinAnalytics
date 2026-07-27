// TODO: way too many separate headers for day counters
#include "time/day_count/day_count_act_365_fixed.h"

#include <cstddef>
#include <tuple>

#include <gtest/gtest.h>

#include "oa/testing/day_count.h"
#include "oa/testing/gtest.h"

namespace {

OA_DECLARE_DAY_COUNT_TEST(
	DayCountAct365FixedTest,
	// day count tests
	std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 1, 30}, 15},
	std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 2, 15}, 31},
	std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 7, 15}, 181},
	std::tuple{std::tuple{2007, 9, 30}, std::tuple{2008, 3, 31}, 183},
	std::tuple{std::tuple{2007, 9, 30}, std::tuple{2007, 10, 31}, 31},
	std::tuple{std::tuple{2007, 9, 30}, std::tuple{2008, 9, 30}, 366},
	std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 1, 31}, 16},
	std::tuple{std::tuple{2007, 1, 31}, std::tuple{2007, 2, 28}, 28},
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 31},
	std::tuple{std::tuple{2006, 8, 31}, std::tuple{2007, 2, 28}, 181},
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 8, 31}, 184},
	std::tuple{std::tuple{2007, 2, 14}, std::tuple{2007, 2, 28}, 14},
	std::tuple{std::tuple{2007, 2, 26}, std::tuple{2008, 2, 29}, 368},
	std::tuple{std::tuple{2008, 2, 29}, std::tuple{2009, 2, 28}, 365},
	std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 30}, 30},
	std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 31}, 31},
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 5}, 5},
	std::tuple{std::tuple{2007, 10, 31}, std::tuple{2007, 11, 28}, 28},
	std::tuple{std::tuple{2007, 8, 31}, std::tuple{2008, 2, 29}, 182},
	std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 8, 31}, 184},
	std::tuple{std::tuple{2008, 8, 31}, std::tuple{2009, 2, 28}, 181},
	std::tuple{std::tuple{2009, 2, 28}, std::tuple{2009, 8, 31}, 184},
	// year fraction tests
	std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 1, 30}, 15 / 365.},
	std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 2, 15}, 31 / 365.},
	std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 7, 15}, 181 / 365.},
	std::tuple{std::tuple{2007, 9, 30}, std::tuple{2008, 3, 31}, 183 / 365.},
	std::tuple{std::tuple{2007, 9, 30}, std::tuple{2007, 10, 31}, 31 / 365.},
	std::tuple{std::tuple{2007, 9, 30}, std::tuple{2008, 9, 30}, 366 / 365.},
	std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 1, 31}, 16 / 365.},
	std::tuple{std::tuple{2007, 1, 31}, std::tuple{2007, 2, 28}, 28 / 365.},
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 31 / 365.},
	std::tuple{std::tuple{2006, 8, 31}, std::tuple{2007, 2, 28}, 181 / 365.},
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 8, 31}, 184 / 365.},
	std::tuple{std::tuple{2007, 2, 14}, std::tuple{2007, 2, 28}, 14 / 365.},
	std::tuple{std::tuple{2007, 2, 26}, std::tuple{2008, 2, 29}, 368 / 365.},
	std::tuple{std::tuple{2008, 2, 29}, std::tuple{2009, 2, 28}, 1.},
	std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 30}, 30 / 365.},
	std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 31}, 31 / 365.},
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 5}, 5 / 365.},
	std::tuple{std::tuple{2007, 10, 31}, std::tuple{2007, 11, 28}, 28 / 365.},
	std::tuple{std::tuple{2007, 8, 31}, std::tuple{2008, 2, 29}, 182 / 365.},
	std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 8, 31}, 184 / 365.},
	std::tuple{std::tuple{2008, 8, 31}, std::tuple{2009, 2, 28}, 181 / 365.},
	std::tuple{std::tuple{2009, 2, 28}, std::tuple{2009, 8, 31}, 184 / 365.}
);

OA_DEFINE_DAY_COUNT_TEST(DayCountAct365FixedTest, oa::time::DayCountAct365Fixed);

}  // namespace
