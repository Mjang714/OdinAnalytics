// TODO: way too many separate headers for day counters
#include "time/day_count/day_count_act_act.h"

#include <cstddef>
#include <tuple>

#include <gtest/gtest.h>

#include "oa/testing/day_count.h"
#include "oa/testing/gtest.h"

namespace {

/**
 * `DayCountActAct` test template.
 *
 * @tparam T `oa::testing::index<I>` for test case `I` or `void`
 */
template <typename T = void>
class DayCountActActTest {
public:
	// base type template for test input I
	template <std::size_t I>
	using base_type = oa::testing::DayCountTestBase<
		DayCountActActTest<oa::testing::index<I>>
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
		std::tuple{std::tuple{2010, 12, 30}, std::tuple{2011, 1, 2}, 3 / 365.},
		std::tuple{
			std::tuple{2011, 12, 30},
			std::tuple{2012, 1, 2},
			2 / 365. + 1 / 366.
		},
		std::tuple{
			std::tuple{2010, 12, 30},
			std::tuple{2013, 1, 2},
			367 / 365. + 1. + 1 / 365.
		}
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
 * @tparam T Test input index in `DayCountActActTest<>::inputs`
 */
template <std::size_t I>
class DayCountActActTest<oa::testing::index<I>>
  : public DayCountActActTest<>::base_type<I> {
public:
	oa::time::DayCountActAct dc;
	static constexpr auto& input = DayCountActActTest<>::input<I>();
};

// instantiate index<I> types
TYPED_TEST_SUITE(
	DayCountActActTest,
	oa::testing::index_types<DayCountActActTest<>::inputs_size>
);

// instantiate test
TYPED_TEST(DayCountActActTest, Test)
{
	(*this)();
}

}  // namespace
