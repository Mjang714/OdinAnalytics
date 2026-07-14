// TODO: way too many separate headers for day counters
#include "time/day_count/day_count_act_360.h"

#include <cstddef>
#include <tuple>

#include <gtest/gtest.h>

#include "oa/testing/day_count.h"
#include "oa/testing/gtest.h"

namespace {

/**
 * `DayCountAct360` test template.
 *
 * @tparam T `oa::testing::index<I>` for test case `I` or `void`
 */
template <typename T = void>
class DayCountAct360Test {
public:
	// base type template for test input I
	template <std::size_t I>
	using base_type = oa::testing::DayCountTestBase<
		DayCountAct360Test<oa::testing::index<I>>
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
	// number of test inputs
	static constexpr auto inputs_size = std::tuple_size_v<decltype(inputs)>;

	// helper to retrieve test input I
	template <std::size_t I>
	static constexpr auto& input() noexcept
	{
		return std::get<I>(inputs);
	}
};

/**
 * Partial specialization for test case input `I`.
 *
 * @tparam T Test input index in `DayCountAct360Test<>::inputs`
 */
template <std::size_t I>
class DayCountAct360Test<oa::testing::index<I>>
  : public DayCountAct360Test<>::base_type<I> {
public:
	oa::time::DayCountAct360 dc;
	static constexpr auto& input = DayCountAct360Test<>::input<I>();
};

// instantiate index<I> types
TYPED_TEST_SUITE(
	DayCountAct360Test,
	oa::testing::index_types<DayCountAct360Test<>::inputs_size>
);

// instantiate test
TYPED_TEST(DayCountAct360Test, Test)
{
	(*this)();
}

}  // namespace
