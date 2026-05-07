#include <gtest/gtest.h>
#include "helpers/utils.h"

#include "enum_mappers/time_enum_mappers.h"


namespace
{
    template <typename T>
    class EnumMappersTest : public ::testing::Test{};

    template<typename T>
    struct enum_name_mapper_test{};

    //template to select the right enum name mapper test based on the type of the input
    template <>
    struct enum_name_mapper_test<oa::time::DayCountRule> {
        auto operator()(const std::string& name) const
        {
            return oa::enum_mappers::MapInputToDayCountEnum(name);
        }
    };

    template <>
    struct enum_name_mapper_test<oa::time::AdjRule> {
        auto operator()(const std::string& name) const
        {
            return oa::enum_mappers::MapInputToDayAdjustEnum(name);
        }
    };

    template <>
    struct enum_name_mapper_test<oa::derived_time::DateDirection> {
        auto operator()(const std::string& name) const
        {
            return oa::enum_mappers::MapInputToDateDir(name);
        }
    };

    template <>
    struct enum_name_mapper_test<oa::derived_time::Frequency> {
        auto operator()(const std::string& name) const
        {
            return oa::enum_mappers::MapInputToFreq(name);
        }
    };

    template <>
    struct enum_name_mapper_test<oa::derived_time::StubType> {
        auto operator()(const std::string& name) const
        {
            return oa::enum_mappers::MapInputToStub(name);
        }
    };

    template <>
    struct enum_name_mapper_test<oa::derived_time::CalcType> {
        auto operator()(const std::string& name) const
        {
            return oa::enum_mappers::MapInputToCalcType(name);
        }
    };

    // helper struct to represent an invalid argument for the enum name mappers. this is just a convenient way to represent the invalid cases in the test input tuple
    template <typename T>
    struct enum_name_mapper_invalid_argument {
        static constexpr auto input = "INVALID";
    };

    // helper to check if a type is an enum_name_mapper_invalid_argument. this is used to determine whether to EXPECT_THROW or EXPECT_EQ in the test
    template <typename T>
    struct is_enum_name_mapper_invalid_argument {
        static constexpr bool value = false;
    };

    // specialization for enum_name_mapper_invalid_argument to set value to true
    template <typename T>
    struct is_enum_name_mapper_invalid_argument<enum_name_mapper_invalid_argument<T>> {
        static constexpr bool value = true;
    };

    // partial specialization of enum_name_mapper_test for enum_name_mapper_invalid_argument to allow us to call the test with the invalid input
    template <typename T>
    struct enum_name_mapper_test<enum_name_mapper_invalid_argument<T>>
    : enum_name_mapper_test<T> {};

    // all the test cases where we map string -> enum for all the different enums in our library. we also include some invalid cases to test that the mappers throw the expected exceptions for invalid input
    constexpr auto enum_mapping_test_inputs = std::make_tuple(
        std::pair{oa::time::DayCountRule::kACT_360, "ACT/360"},
        std::pair{oa::time::DayCountRule::kACT_360, "act/360"},
        std::pair{oa::time::DayCountRule::kACT_360, "ACT:360"},
        std::pair{oa::time::DayCountRule::kACT_360, "act:360"},
        std::pair{oa::time::DayCountRule::kACT_360, "ACT_360"},
        std::pair{oa::time::DayCountRule::kACT_360, "act_360"},
        std::pair{oa::time::DayCountRule::kACT_360, "ACT360"},
        std::pair{oa::time::DayCountRule::kACT_360, "act360"},
        std::pair{oa::time::DayCountRule::k30_360_BOND_BASIS, "30:360"},
        std::pair{oa::time::DayCountRule::k30_360_BOND_BASIS, "30/360"},
        std::pair{oa::time::DayCountRule::k30_360_BOND_BASIS, "30_360"},
        std::pair{oa::time::DayCountRule::k30_360_BOND_BASIS, "30360"},
        std::pair{oa::time::DayCountRule::k30_E_360_ISDA, "30E/360"},
        std::pair{oa::time::DayCountRule::k30_E_360_ISDA, "30E:360"},
        std::pair{oa::time::DayCountRule::k30_E_360_ISDA, "30E_360"},
        std::pair{oa::time::DayCountRule::k30_E_360_ISDA, "30E360"},
        std::pair{oa::time::DayCountRule::k30_E_360_ISDA, "30e/360"},
        std::pair{oa::time::DayCountRule::k30_E_360_ISDA, "30e:360"},
        std::pair{oa::time::DayCountRule::k30_E_360_ISDA, "30e_360"},
        std::pair{oa::time::DayCountRule::k30_E_360_ISDA, "30e360"},
        std::pair{oa::time::DayCountRule::kACT_365_FIXED, "ACT/365"},
        std::pair{oa::time::DayCountRule::kACT_365_FIXED, "ACT:365"},
        std::pair{oa::time::DayCountRule::kACT_365_FIXED, "ACT_365"},
        std::pair{oa::time::DayCountRule::kACT_365_FIXED, "ACT365"},
        std::pair{oa::time::DayCountRule::kACT_365_FIXED, "act/365"},
        std::pair{oa::time::DayCountRule::kACT_365_FIXED, "act:365"},
        std::pair{oa::time::DayCountRule::kACT_365_FIXED, "act_365"},
        std::pair{oa::time::DayCountRule::kACT_365_FIXED, "act365"},   
        std::pair{oa::time::DayCountRule::kACT_ACT, "ACT/ACT"},
        std::pair{oa::time::DayCountRule::kACT_ACT, "ACT:ACT"},
        std::pair{oa::time::DayCountRule::kACT_ACT, "ACT_ACT"},
        std::pair{oa::time::DayCountRule::kACT_ACT, "ACTACT"},
        std::pair{oa::time::DayCountRule::kACT_ACT, "act/act"},
        std::pair{oa::time::DayCountRule::kACT_ACT, "act:act"},
        std::pair{oa::time::DayCountRule::kACT_ACT, "act_act"},
        std::pair{oa::time::DayCountRule::kACT_ACT, "actact"},
        // ...
        enum_name_mapper_invalid_argument<oa::time::DayCountRule>{},
        std::pair{oa::time::AdjRule::kModifiedFollowing, "MF"},
        std::pair{oa::time::AdjRule::kModifiedFollowing, "MODIFIEDFOLLOWING"},
        std::pair{oa::time::AdjRule::kModifiedFollowing, "MODIFIED_FOLLOWING"},
        std::pair{oa::time::AdjRule::kModifiedFollowing, "MODF"},
        std::pair{oa::time::AdjRule::kModifiedFollowing, "MFOL"},
        std::pair{oa::time::AdjRule::kPreceding, "P"},
        std::pair{oa::time::AdjRule::kPreceding, "PRECEDING"},
        std::pair{oa::time::AdjRule::kPreceding, "PRE"},
        std::pair{oa::time::AdjRule::kFollowing, "F"},
        std::pair{oa::time::AdjRule::kFollowing, "FOLLOWING"},
        std::pair{oa::time::AdjRule::kFollowing, "FOL"},
        std::pair{oa::time::AdjRule::kPlainAdjustment, "NOADJ"},
        std::pair{oa::time::AdjRule::kPlainAdjustment, "NO_ADJ"},
        std::pair{oa::time::AdjRule::kPlainAdjustment, "NONE"},
        std::pair{oa::time::AdjRule::kModifiedFollowing, "mf"},
        std::pair{oa::time::AdjRule::kModifiedFollowing, "modifiedfollowing"},
        std::pair{oa::time::AdjRule::kModifiedFollowing, "modified_following"},
        std::pair{oa::time::AdjRule::kModifiedFollowing, "modf"},
        std::pair{oa::time::AdjRule::kModifiedFollowing, "mfol"},
        std::pair{oa::time::AdjRule::kPreceding, "p"},
        std::pair{oa::time::AdjRule::kPreceding, "preceding"},
        std::pair{oa::time::AdjRule::kPreceding, "pre"},
        std::pair{oa::time::AdjRule::kFollowing, "f"},
        std::pair{oa::time::AdjRule::kFollowing, "following"},
        std::pair{oa::time::AdjRule::kFollowing, "fol"},
        std::pair{oa::time::AdjRule::kPlainAdjustment, "noadj"},
        std::pair{oa::time::AdjRule::kPlainAdjustment, "no_adj"},
        std::pair{oa::time::AdjRule::kPlainAdjustment, "none"},   
        // ...
        enum_name_mapper_invalid_argument<oa::time::AdjRule>{},
        std::pair{oa::derived_time::DateDirection::kForward, "FORWARD"},
        std::pair{oa::derived_time::DateDirection::kForward, "forward"},
        std::pair{oa::derived_time::DateDirection::kForward, "FWD"},
        std::pair{oa::derived_time::DateDirection::kForward, "fwd"},
        std::pair{oa::derived_time::DateDirection::kBackward, "BACKWARD"},
        std::pair{oa::derived_time::DateDirection::kBackward, "backward"},
        std::pair{oa::derived_time::DateDirection::kBackward, "BCKWD"},
        std::pair{oa::derived_time::DateDirection::kBackward, "bckwd"},
        std::pair{oa::derived_time::DateDirection::kBackward, "BACKWARDS"},
        std::pair{oa::derived_time::DateDirection::kBackward, "backwards"},
        // ...
        enum_name_mapper_invalid_argument<oa::derived_time::DateDirection>{},
        std::pair{oa::derived_time::Frequency::kOnce, "ONCE"},
        std::pair{oa::derived_time::Frequency::kOnce, "once"},
        std::pair{oa::derived_time::Frequency::kOnce, "O"},
        std::pair{oa::derived_time::Frequency::kOnce, "o"},
        std::pair{oa::derived_time::Frequency::kAnnual, "ANNUAL"},
        std::pair{oa::derived_time::Frequency::kAnnual, "annual"},
        std::pair{oa::derived_time::Frequency::kAnnual, "A"},
        std::pair{oa::derived_time::Frequency::kAnnual, "a"},
        std::pair{oa::derived_time::Frequency::kSemiAnnual, "SEMIANNUAL"},
        std::pair{oa::derived_time::Frequency::kSemiAnnual, "semiannual"},
        std::pair{oa::derived_time::Frequency::kSemiAnnual, "SA"},
        std::pair{oa::derived_time::Frequency::kSemiAnnual, "sa"},
        std::pair{oa::derived_time::Frequency::kQuarterly, "QUARTERLY"},
        std::pair{oa::derived_time::Frequency::kQuarterly, "quarterly"},
        std::pair{oa::derived_time::Frequency::kQuarterly, "Q"},
        std::pair{oa::derived_time::Frequency::kQuarterly, "q"},
        std::pair{oa::derived_time::Frequency::kMonthly, "MONTHLY"},
        std::pair{oa::derived_time::Frequency::kMonthly, "monthly"},
        std::pair{oa::derived_time::Frequency::kMonthly, "M"},
        std::pair{oa::derived_time::Frequency::kMonthly, "m"},
        std::pair{oa::derived_time::Frequency::kWeekly, "WEEKLY"},
        std::pair{oa::derived_time::Frequency::kWeekly, "weekly"},
        std::pair{oa::derived_time::Frequency::kWeekly, "W"},
        std::pair{oa::derived_time::Frequency::kWeekly, "w"},
        std::pair{oa::derived_time::Frequency::kDaily, "DAILY"},
        std::pair{oa::derived_time::Frequency::kDaily, "daily"},
        std::pair{oa::derived_time::Frequency::kDaily, "D"},
        std::pair{oa::derived_time::Frequency::kDaily, "d"},    
        // ...
        enum_name_mapper_invalid_argument<oa::derived_time::Frequency>{},
        std::pair{oa::derived_time::StubType::kNone, "NONE"},
        std::pair{oa::derived_time::StubType::kNone, "none"},
        std::pair{oa::derived_time::StubType::kShortFirst, "SHORTFIRST"},
        std::pair{oa::derived_time::StubType::kShortFirst, "shortfirst"},
        std::pair{oa::derived_time::StubType::kShortFirst, "SF"},
        std::pair{oa::derived_time::StubType::kShortFirst, "sf"},
        std::pair{oa::derived_time::StubType::kLongFirst, "LONGFIRST"},
        std::pair{oa::derived_time::StubType::kLongFirst, "longfirst"},
        std::pair{oa::derived_time::StubType::kLongFirst, "LF"},
        std::pair{oa::derived_time::StubType::kLongFirst, "lf"},
        std::pair{oa::derived_time::StubType::kLongLast, "LONGLAST"},
        std::pair{oa::derived_time::StubType::kLongLast, "longlast"},
        std::pair{oa::derived_time::StubType::kLongLast, "LL"},
        std::pair{oa::derived_time::StubType::kLongLast, "ll"},
        // ...
        enum_name_mapper_invalid_argument<oa::derived_time::StubType>{},
        std::pair{oa::derived_time::CalcType::kFlat, "FLAT"},
        std::pair{oa::derived_time::CalcType::kFlat, "flat"},
        std::pair{oa::derived_time::CalcType::kBBGCalcType1, "BBG1"},
        std::pair{oa::derived_time::CalcType::kBBGCalcType1, "bbg1"},
        std::pair{oa::derived_time::CalcType::kBBGCalcType2, "BBG2"},
        std::pair{oa::derived_time::CalcType::kBBGCalcType2, "bbg2"},
        std::pair{oa::derived_time::CalcType::kUSTStreetConv, "USTSTREETCONV"},
        std::pair{oa::derived_time::CalcType::kUSTStreetConv, "uststreetconv"},
        // ...
        enum_name_mapper_invalid_argument<oa::derived_time::CalcType>{}
    );

    // partial specialization for the mapping tests that does the real work
    // note: index_sequence used for compile-time index value in a type
    template <std::size_t I>
    class EnumMappersTest<std::index_sequence<I>> : public ::testing::Test {
    protected:
    // ensure you can't compile a test that is out-of-bounds
    static_assert(I <= std::tuple_size_v<decltype(enum_mapping_test_inputs)>);
    // test
    void operator()() const
    {
        // reference to input + type
        const auto& input = std::get<I>(enum_mapping_test_inputs);
        using input_type = std::decay_t<decltype(input)>;
        // if enum_name_mapper_invalid_argument use EXPECT_THROW
        if constexpr (is_enum_name_mapper_invalid_argument<input_type>::value)
            EXPECT_THROW(
                enum_name_mapper_test<input_type>{}(input_type::input),
                std::invalid_argument
            );
        // otherwise perform normal test
        else {
            // get type of input.first (use decay for convenience)
            using enum_type = std::decay_t<decltype(input.first)>;
            // perform check using enum_name_mapper
            EXPECT_EQ(input.first, enum_name_mapper_test<enum_type>{}(input.second));
        }
    }
};

    // helper template to construct ::testing::Types<...> of index_sequence
    template <typename T>
    struct enum_name_mapper_types_impl {};

    // partial specialization to turn index_sequence<Is...> -> index_sequence<Is>...
    template <std::size_t... Is>
    struct enum_name_mapper_types_impl<std::index_sequence<Is...>> {
        using type = ::testing::Types<std::index_sequence<Is>...>;
    };

    // typed test types template
    template <std::size_t N>
    using enum_name_mapper_types =
    typename enum_name_mapper_types_impl<std::make_index_sequence<N>>::type;

    // instantiate test types
    TYPED_TEST_SUITE(
        EnumMappersTest,
        enum_name_mapper_types<std::tuple_size_v<decltype(enum_mapping_test_inputs)>>
    );

    // instantiate single test
    TYPED_TEST(EnumMappersTest, Test)
    {
        (*this)();
    };
}