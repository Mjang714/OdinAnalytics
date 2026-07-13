// TODO: way too many separate headers for day counters
#include "time/day_count/day_count_act_365_fixed.h"

#include <cstddef>
#include <tuple>

#include <gtest/gtest.h>

#include "oa/testing/day_count.h"
#include "oa/testing/gtest.h"

namespace {

/**
 * `DayCountAct365Fixed` test template.
 *
 * @tparam T `oa::testing::index<I>` for test case `I` or `void`
 */
template <typename T = void>
class DayCountAct365FixedTest {
public:
	// convenience type alias for the CRTP base type corresponding to test I
	template <std::size_t I>
	using base_type = oa::testing::DayCountTestBase<
		DayCountAct365FixedTest<oa::testing::index<I>>
	>;

	// test inputs
	static constexpr auto inputs = std::make_tuple(
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
	// number of test inputs for index_types
	static constexpr auto inputs_size = std::tuple_size_v<decltype(inputs)>;

	// helper to retrieve input I
	template <std::size_t I>
	static constexpr auto& input() noexcept
	{
		return std::get<I>(inputs);
	}
};

/**
 * Partial specialization for test case input `I`.
 *
 * @tparam T Test input index in `DayCountAct365FixedTest<>::inputs`
 */
template <std::size_t I>
class DayCountAct365FixedTest<oa::testing::index<I>>
  : public DayCountAct365FixedTest<>::base_type<I> {
public:
	oa::time::DayCountAct365Fixed dc;
	static constexpr auto& input = DayCountAct365FixedTest<>::input<I>();
};

// instantiate index<I> types
TYPED_TEST_SUITE(
	DayCountAct365FixedTest,
	oa::testing::index_types<DayCountAct365FixedTest<>::inputs_size>
);

// instantiate test
OA_DAY_COUNT_TEST(DayCountAct365FixedTest);

}  // namespace
