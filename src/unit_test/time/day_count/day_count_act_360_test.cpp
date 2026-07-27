// TODO: way too many separate headers for day counters
#include "time/day_count/day_count_act_360.h"

#include <cstddef>
#include <tuple>

#include <gtest/gtest.h>

#include "oa/testing/day_count.h"
#include "oa/testing/gtest.h"

namespace {

OA_DECLARE_DAY_COUNT_TEST(
	DayCountAct360Test,
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
	std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 1, 30}, 15 / 360.},
	std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 2, 15}, 31 / 360.},
	std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 7, 15}, 181 / 360.},
	std::tuple{std::tuple{2007, 9, 30}, std::tuple{2008, 3, 31}, 183 / 360.},
	std::tuple{std::tuple{2007, 9, 30}, std::tuple{2007, 10, 31}, 31 / 360.},
	std::tuple{std::tuple{2007, 9, 30}, std::tuple{2008, 9, 30}, 366 / 360.},
	std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 1, 31}, 16 / 360.},
	std::tuple{std::tuple{2007, 1, 31}, std::tuple{2007, 2, 28}, 28 / 360.},
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 31 / 360.},
	std::tuple{std::tuple{2006, 8, 31}, std::tuple{2007, 2, 28}, 181 / 360.},
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 8, 31}, 184 / 360.},
	std::tuple{std::tuple{2007, 2, 14}, std::tuple{2007, 2, 28}, 14 / 360.},
	std::tuple{std::tuple{2007, 2, 26}, std::tuple{2008, 2, 29}, 368 / 360.},
	std::tuple{std::tuple{2008, 2, 29}, std::tuple{2009, 2, 28}, 365 / 360.},
	std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 30}, 30 / 360.},
	std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 31}, 31 / 360.},
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 5}, 5 / 360.},
	std::tuple{std::tuple{2007, 10, 31}, std::tuple{2007, 11, 28}, 28 / 360.},
	std::tuple{std::tuple{2007, 8, 31}, std::tuple{2008, 2, 29}, 182 / 360.},
	std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 8, 31}, 184 / 360.},
	std::tuple{std::tuple{2008, 8, 31}, std::tuple{2009, 2, 28}, 181 / 360.},
	std::tuple{std::tuple{2009, 2, 28}, std::tuple{2009, 8, 31}, 184 / 360.}
);

OA_DEFINE_DAY_COUNT_TEST(DayCountAct360Test, oa::time::DayCountAct360);

}  // namespace
