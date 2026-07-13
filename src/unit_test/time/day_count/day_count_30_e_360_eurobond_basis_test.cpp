// TODO: way too many separate headers for day counters
#include "time/day_count/day_count_30_e_360_eurobond_basis.h"

#include <cstddef>
#include <tuple>

#include <gtest/gtest.h>

#include "oa/testing/day_count.h"
#include "oa/testing/gtest.h"

namespace {

/**
 * `DayCount30E360EuroBondBasis` test template.
 *
 * @tparam T `oa::testing::index<I>` for test case `I` or `void`
 */
template <typename T = void>
class DayCount30E360Test {
public:
	// base type template for test input I
	template <std::size_t I>
	using base_type = oa::testing::DayCountTestBase<
		DayCount30E360Test<oa::testing::index<I>>
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
		std::tuple{std::tuple{2007, 1, 31}, std::tuple{2007, 2, 28}, 28},
		std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 32},
		std::tuple{std::tuple{2006, 8, 31}, std::tuple{2007, 2, 28}, 178},
		std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 8, 31}, 182},
		std::tuple{std::tuple{2007, 2, 14}, std::tuple{2007, 2, 28}, 14},
		std::tuple{std::tuple{2007, 2, 26}, std::tuple{2008, 2, 29}, 363},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2009, 2, 28}, 359},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 30}, 31},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 31}, 31},
		std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 5}, 7},
		std::tuple{std::tuple{2007, 10, 31}, std::tuple{2007, 11, 28}, 28},
		std::tuple{std::tuple{2007, 8, 31}, std::tuple{2008, 2, 29}, 179},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 8, 31}, 181},
		std::tuple{std::tuple{2008, 8, 31}, std::tuple{2009, 2, 28}, 178},
		std::tuple{std::tuple{2009, 2, 28}, std::tuple{2009, 8, 31}, 182},
		// year fraction tests
		std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 1, 30}, 15 / 360.},
		std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 2, 15}, 30 / 360.},
		std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 7, 15}, .5},
		std::tuple{std::tuple{2007, 9, 30}, std::tuple{2008, 3, 31}, .5},
		std::tuple{std::tuple{2007, 9, 30}, std::tuple{2007, 10, 31}, 30 / 360.},
		std::tuple{std::tuple{2007, 9, 30}, std::tuple{2008, 9, 30}, 1.},
		std::tuple{std::tuple{2007, 1, 15}, std::tuple{2007, 1, 31}, 15 / 360.},
		std::tuple{std::tuple{2007, 1, 31}, std::tuple{2007, 2, 28}, 28 / 360.},
		std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 32 / 360.},
		std::tuple{std::tuple{2006, 8, 31}, std::tuple{2007, 2, 28}, 178 / 360.},
		std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 8, 31}, 182 / 360.},
		std::tuple{std::tuple{2007, 2, 14}, std::tuple{2007, 2, 28}, 14 / 360.},
		std::tuple{std::tuple{2007, 2, 26}, std::tuple{2008, 2, 29}, 363 / 360.},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2009, 2, 28}, 359 / 360.},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 30}, 31 / 360.},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 3, 31}, 31 / 360.},
		std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 5}, 7 / 360.},
		std::tuple{std::tuple{2007, 10, 31}, std::tuple{2007, 11, 28}, 28 / 360.},
		std::tuple{std::tuple{2007, 8, 31}, std::tuple{2008, 2, 29}, 179 / 360.},
		std::tuple{std::tuple{2008, 2, 29}, std::tuple{2008, 8, 31}, 181 / 360.},
		std::tuple{std::tuple{2008, 8, 31}, std::tuple{2009, 2, 28}, 178 / 360.},
		std::tuple{std::tuple{2009, 2, 28}, std::tuple{2009, 8, 31}, 182 / 360.}
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
 * @tparam T Test input index in `DayCount30E360Test<>::inputs`
 */
template <std::size_t I>
class DayCount30E360Test<oa::testing::index<I>>
  : public DayCount30E360Test<>::base_type<I> {
public:
	oa::time::DayCount30E360EuroBondBasis dc;
	static constexpr auto& input = DayCount30E360Test<>::input<I>();
};

// instantiate index<I> types
TYPED_TEST_SUITE(
	DayCount30E360Test,
	oa::testing::index_types<DayCount30E360Test<>::inputs_size>
);

// instantiate test
TYPED_TEST(DayCount30E360Test, Test)
{
	(*this)();
}

}  // namespace
