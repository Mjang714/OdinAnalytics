// TODO: way too many separate headers for day counters
#include "time/day_count/day_counter_factory.h"

#include <cstddef>
#include <memory>
#include <tuple>

#include <gtest/gtest.h>

#include "oa/testing/day_count.h"
#include "oa/testing/gtest.h"
#include "time/day_count/day_count_base.h"
#include "time/day_count/day_count_30_360_bond_basis.h"
#include "time/day_count/day_count_30_360e_isda.h"
#include "time/day_count/day_count_30_e_360_eurobond_basis.h"
#include "time/day_count/day_count_act_360.h"
#include "time/day_count/day_count_act_act.h"
#include "time/day_count/day_count_act_365_fixed.h"

namespace {

/**
 * Class template mixin to generate the day counter instance.
 *
 * @tparam type `DayCountRule` enumerator
 */
template <oa::time::DayCountRule type>
class DayCounterGenerator {
public:
	/**
	 * Return the appropriate day counter instance given the enum type.
	 */
	auto counter() const
	{
		return oa::time::DayCounterFactory::GenerateDayCounter(type);
	}
};

/**
 * Declare a day count factory test with the given name and inputs.
 *
 * This defines the base GoogleTest test class template to hold the tuple of
 * inputs as well as help members and member functions.
 *
 * @note `test_name` cannot be a macro due to a shortcoming in GoogleTest's
 *  implementation of `TYPED_TEST_SUITE()` and `TYPED_TEST()`.
 *
 * @param test_name GoogleTest test class name
 * @param ... Test inputs of type
 *  `std::tuple<std::tuple<int, int, int>, std::tuple<int, int, int>, int>` or
 *  `std::tuple<std::tuple<int, int, int>, std::tuple<int, int, int>, double>`
 */
#define OA_DECLARE_DAY_COUNT_FACTORY_TEST(test_name, ...) \
	/* base template for managing test inputs + other data */ \
	template <typename T = void> \
	class test_name { \
	public: \
		/* base type template for input I */ \
		template <std::size_t I> \
		using base_type = oa::testing::DayCountTestBase< \
			test_name<oa::testing::index<I>> \
		>; \
		\
		/* test inputs + number of test inputs */ \
		static constexpr auto inputs = std::make_tuple(__VA_ARGS__); \
		static constexpr auto inputs_size = std::tuple_size_v<decltype(inputs)>; \
		\
		/* helper to retrieve test input I */ \
		template <std::size_t I> \
		static constexpr auto& input() noexcept \
		{ \
			return std::get<I>(inputs); \
		} \
	}

/**
 * Define the day count factory test with the given name and day count rule.
 *
 * This defines the `oa::testing::index<I>` partial specialization for the test
 * class template, instantiates the types for `TYPED_TEST_SUITE()`, and defines
 * a test named `Test` using `TYPED_TEST()` for the test class template.
 *
 * @note `test_name` cannot be a macro due to a shortcoming in GoogleTest's
 *  implementation of `TYPED_TEST_SUITE()` and `TYPED_TEST()`.
 *
 * @param test_name GoogleTest test class name
 * @param rule `oa::time::DayCountRule` enumerator value
 */
#define OA_DEFINE_DAY_COUNT_FACTORY_TEST(test_name, rule) \
	/* partial specialization for each test case I */ \
	template <std::size_t I> \
	class test_name<oa::testing::index<I>> \
	  : public test_name<>::base_type<I>, \
		public DayCounterGenerator<rule> { \
	public: \
		std::unique_ptr<oa::time::DayCounterBase> dc = counter(); \
		static constexpr auto& input = test_name<>::input<I>(); \
	}; \
	\
	/* instantiate index<I> types */ \
	TYPED_TEST_SUITE( \
		test_name, \
		oa::testing::index_types<test_name<>::inputs_size> \
	); \
	\
	/* instantiate test */ \
	TYPED_TEST(test_name, Test) \
	{ \
		(*this)(); \
	} \
	/* enforce terminating semicolon */ \
	static_assert(true)

////////////////////////////////////////////////////////////////////////////////
// kACT_ACT tests                                                             //
////////////////////////////////////////////////////////////////////////////////

OA_DECLARE_DAY_COUNT_FACTORY_TEST(
	DayCountFactoryActActTest,
	// day count tests
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 31},
	std::tuple{std::tuple{2007, 3, 31}, std::tuple{2007, 2, 28}, -31}
);

OA_DEFINE_DAY_COUNT_FACTORY_TEST(
	DayCountFactoryActActTest,
	oa::time::DayCountRule::kACT_ACT
);

////////////////////////////////////////////////////////////////////////////////
// kACT_360 tests                                                             //
////////////////////////////////////////////////////////////////////////////////

OA_DECLARE_DAY_COUNT_FACTORY_TEST(
	DayCountFactoryAct360Test,
	// day count tests
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 31},
	std::tuple{std::tuple{2007, 3, 31}, std::tuple{2007, 2, 28}, -31},
	// year fraction tests
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 31 / 360.}
);

OA_DEFINE_DAY_COUNT_FACTORY_TEST(
	DayCountFactoryAct360Test,
	oa::time::DayCountRule::kACT_360
);

////////////////////////////////////////////////////////////////////////////////
// kACT_365_FIXED tests                                                       //
////////////////////////////////////////////////////////////////////////////////

OA_DECLARE_DAY_COUNT_FACTORY_TEST(
	DayCountFactoryAct365Test,
	// day count tests
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 31},
	std::tuple{std::tuple{2007, 3, 31}, std::tuple{2007, 2, 28}, -31},
	// year fraction tests
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 31 / 365.}
);

OA_DEFINE_DAY_COUNT_FACTORY_TEST(
	DayCountFactoryAct365Test,
	oa::time::DayCountRule::kACT_365_FIXED
);

////////////////////////////////////////////////////////////////////////////////
// k30_360_BOND_BASIS tests                                                   //
////////////////////////////////////////////////////////////////////////////////

OA_DECLARE_DAY_COUNT_FACTORY_TEST(
	DayCountFactory30360Test,
	// day count tests
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 33},
	std::tuple{std::tuple{2007, 3, 31}, std::tuple{2007, 2, 28}, -33},
	// year fraction tests
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 33 / 360.}
);

OA_DEFINE_DAY_COUNT_FACTORY_TEST(
	DayCountFactory30360Test,
	oa::time::DayCountRule::k30_360_BOND_BASIS
);

////////////////////////////////////////////////////////////////////////////////
// k30_360_E_EUROBOND tests                                                   //
////////////////////////////////////////////////////////////////////////////////

OA_DECLARE_DAY_COUNT_FACTORY_TEST(
	DayCountFactory30360ETest,
	// day count tests
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 32},
	std::tuple{std::tuple{2007, 3, 31}, std::tuple{2007, 2, 28}, -32},
	// year fraction tests
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 32 / 360.}
);

OA_DEFINE_DAY_COUNT_FACTORY_TEST(
	DayCountFactory30360ETest,
	oa::time::DayCountRule::k30_360_E_EUROBOND
);

////////////////////////////////////////////////////////////////////////////////
// k30_E_360_ISDA tests                                                       //
////////////////////////////////////////////////////////////////////////////////

OA_DECLARE_DAY_COUNT_FACTORY_TEST(
	DayCountFactory30E360Test,
	// day count tests
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 30},
	std::tuple{std::tuple{2007, 3, 31}, std::tuple{2007, 2, 28}, -30},
	// year fraction tests
	std::tuple{std::tuple{2007, 2, 28}, std::tuple{2007, 3, 31}, 30 / 360.}
);

OA_DEFINE_DAY_COUNT_FACTORY_TEST(
	DayCountFactory30E360Test,
	oa::time::DayCountRule::k30_E_360_ISDA
);

}  // namespace
