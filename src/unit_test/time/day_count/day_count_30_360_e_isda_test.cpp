// TODO: way too many separate headers for day counters
#include "time/day_count/day_count_30_360e_isda.h"

#include <cstddef>
#include <tuple>

#include <gtest/gtest.h>

#include "oa/testing/day_count.h"
#include "oa/testing/gtest.h"

namespace {

/**
 * `DayCount30360EISDA` test template.
 *
 * @tparam T `oa::testing::index<I>` for test case `I` or `void`
 */
template <typename = void>
class DayCount30360EISDATest {
public:
	// base type template for test input I
	template <std::size_t I>
	using base_type = oa::testing::DayCountTestBase<
		DayCount30360EISDATest<oa::testing::index<I>>
	>;

	// test inputs
	static constexpr auto inputs = std::make_tuple(
		// day count tests
		std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 1, 30}, 15},
		std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 2, 15}, 30},
		std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 7, 15}, 180},
		std::tuple{std::tuple{2007, 9, 30}, std::tuple{2008, 3, 31}, 180},
		std::tuple{std::tuple{2007, 9, 30}, std::tuple{2007, 10, 31}, 30},
		std::tuple{std::tuple{2007, 9, 30}, std::tuple{2008, 9, 30}, 360},
		std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 1, 31}, 15},
		std::tuple{std::tuple{2007, 1, 31}, std::tuple{2007, 2, 28}, 30},
		std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 30},
		std::tuple{std::tuple{2006, 8, 31}, std::tuple{2007, 2, 28}, 180},
		std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 8, 31}, 180},
		std::tuple{std::tuple{2007, 2, 14}, std::tuple{2007, 2, 28}, 16},
		std::tuple{std::tuple{2007, 2, 26}, std::tuple{2008, 2, 29}, 364},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2009, 2, 28}, 360},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 30}, 30},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 31}, 30},
		std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 5}, 5},
		std::tuple{std::tuple{2007, 10, 31}, std::tuple{2007, 11, 28}, 28},
		std::tuple{std::tuple{2007, 8, 31}, std::tuple{2008, 2, 29}, 180},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 8, 31}, 180},
		std::tuple{std::tuple{2008, 8, 31}, std::tuple{2009, 2, 28}, 180},
		std::tuple{std::tuple{2009, 2, 28}, std::tuple{2009, 8, 31}, 180},
		// year fraction tests
		std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 1, 30}, 15 / 360.},
		std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 2, 15}, 30 / 360.},
		std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 7, 15}, .5},
		std::tuple{std::tuple{2007, 9, 30}, std::tuple{2008, 3, 31}, .5},
		std::tuple{std::tuple{2007, 9, 30}, std::tuple{2007, 10, 31}, 30 / 360.},
		std::tuple{std::tuple{2007, 9, 30}, std::tuple{2008, 9, 30}, 1.},
		std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 1, 31}, 15 / 360.},
		std::tuple{std::tuple{2007, 1, 31}, std::tuple{2007, 2, 28}, 30 / 360.},
		std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 30 / 360.},
		std::tuple{std::tuple{2006, 8, 31}, std::tuple{2007, 2, 28}, .5},
		std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 8, 31}, .5},
		std::tuple{std::tuple{2007, 2, 14}, std::tuple{2007, 2, 28}, 16 / 360.},
		std::tuple{std::tuple{2007, 2, 26}, std::tuple{2008, 2, 29}, 364 / 360.},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2009, 2, 28}, 1.},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 30}, 30 / 360.},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 31}, 30 / 360.},
		std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 5}, 5 / 360.},
		std::tuple{std::tuple{2007, 10, 31}, std::tuple{2007, 11, 28}, 28 / 360.},
		std::tuple{std::tuple{2007, 8, 31}, std::tuple{2008, 2, 29}, .5},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 8, 31}, .5},
		std::tuple{std::tuple{2008, 8, 31}, std::tuple{2009, 2, 28}, .5},
		std::tuple{std::tuple{2009, 2, 28}, std::tuple{2009, 8, 31}, .5}
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
 * @tparam I Test input index in `DayCount30360EISDATest<>::inputs`
 */
template <std::size_t I>
class DayCount30360EISDATest<oa::testing::index<I>>
  : public DayCount30360EISDATest<>::base_type<I> {
public:
	oa::time::DayCount30360EISDA dc;
	static constexpr auto& input = DayCount30360EISDATest<>::input<I>();
};

// instantiate index<I> types
TYPED_TEST_SUITE(
	DayCount30360EISDATest,
	oa::testing::index_types<DayCount30360EISDATest<>::inputs_size>
);

// instantiate test
TYPED_TEST(DayCount30360EISDATest, Test)
{
	(*this)();
}

}  // namespace
