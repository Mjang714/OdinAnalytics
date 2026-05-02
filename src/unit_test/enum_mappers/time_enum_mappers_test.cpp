#include "gtest/gtest.h"
#include "helpers/utils.h"

#include "enum_mappers/time_enum_mappers.h"


namespace
{
    TEST(TimeEnumMappersTest, MapInputToDayCountEnumTest)
    {
        EXPECT_EQ(oa::time::DayCountRule::kACT_360, oa::enum_mappers::MapInputToDayCountEnum("ACT/360"));
        EXPECT_EQ(oa::time::DayCountRule::k30_E_360_ISDA, oa::enum_mappers::MapInputToDayCountEnum("30E/360"));
        EXPECT_EQ(oa::time::DayCountRule::k30_360_BOND_BASIS, oa::enum_mappers::MapInputToDayCountEnum("30/360"));
        EXPECT_EQ(oa::time::DayCountRule::k30_360_E_EUROBOND, oa::enum_mappers::MapInputToDayCountEnum("30E/360EURO"));
        EXPECT_EQ(oa::time::DayCountRule::kACT_ACT, oa::enum_mappers::MapInputToDayCountEnum("ACT/ACT"));
    }

    TEST(TimeEnumMappersTest, MapInputToDayAdjustEnumTest)
    {
        EXPECT_EQ(oa::time::AdjRule::kModifiedFollowing, oa::enum_mappers::MapInputToDayAdjustEnum("MF"));
        EXPECT_EQ(oa::time::AdjRule::kPreceding, oa::enum_mappers::MapInputToDayAdjustEnum("P"));
        EXPECT_EQ(oa::time::AdjRule::kFollowing, oa::enum_mappers::MapInputToDayAdjustEnum("F"));
    }

    TEST(TimeEnumMappersTest, MapInputToDayCountEnumCaseInsensitiveTest)
    {
        EXPECT_EQ(oa::time::DayCountRule::kACT_360, oa::enum_mappers::MapInputToDayCountEnum("act/360"));
        EXPECT_EQ(oa::time::DayCountRule::k30_E_360_ISDA, oa::enum_mappers::MapInputToDayCountEnum("30e/360"));
        EXPECT_EQ(oa::time::DayCountRule::k30_360_BOND_BASIS, oa::enum_mappers::MapInputToDayCountEnum("30/360"));
        EXPECT_EQ(oa::time::DayCountRule::k30_360_E_EUROBOND, oa::enum_mappers::MapInputToDayCountEnum("30e/360euro"));
        EXPECT_EQ(oa::time::DayCountRule::kACT_ACT, oa::enum_mappers::MapInputToDayCountEnum("act/act"));
    }
    
    TEST(TimeEnumMappersTest, MapInputToDayAdjustEnumCaseInsensitiveTest)
    {
        EXPECT_EQ(oa::time::AdjRule::kModifiedFollowing, oa::enum_mappers::MapInputToDayAdjustEnum("mf"));
        EXPECT_EQ(oa::time::AdjRule::kPreceding, oa::enum_mappers::MapInputToDayAdjustEnum("p"));
        EXPECT_EQ(oa::time::AdjRule::kFollowing, oa::enum_mappers::MapInputToDayAdjustEnum("f"));
    }

    TEST(TimeEnumMappersTest, MapInputToDayCountEnumInvalidInputTest)
    {
        EXPECT_THROW(oa::enum_mappers::MapInputToDayCountEnum("INVALID"), std::invalid_argument);
    }

    TEST(TimeEnumMappersTest, MapInputToDayAdjustEnumInvalidInputTest)
    {
        EXPECT_THROW(oa::enum_mappers::MapInputToDayAdjustEnum("INVALID"), std::invalid_argument);
    }

    TEST(DateDirectionTest, MapInputToDateDirTest)
    {
        EXPECT_EQ(oa::derived_time::DateDirection::kForward, oa::enum_mappers::MapInputToDateDir("FORWARD"));
        EXPECT_EQ(oa::derived_time::DateDirection::kBackward, oa::enum_mappers::MapInputToDateDir("BACKWARD"));
        EXPECT_EQ(oa::derived_time::DateDirection::kBackward, oa::enum_mappers::MapInputToDateDir("BACKWARDS"));
    }

    TEST(DateDirectionTest, MapInputToDateDirCaseInsensitiveTest)
    {
        EXPECT_EQ(oa::derived_time::DateDirection::kForward, oa::enum_mappers::MapInputToDateDir("forward"));
        EXPECT_EQ(oa::derived_time::DateDirection::kBackward, oa::enum_mappers::MapInputToDateDir("backward"));
        EXPECT_EQ(oa::derived_time::DateDirection::kBackward, oa::enum_mappers::MapInputToDateDir("backwards"));
    }

    TEST(DateDirectionTest, MapInputToDateDirInvalidInputTest)
    {
        EXPECT_THROW(oa::enum_mappers::MapInputToDateDir("INVALID"), std::invalid_argument);
    }

    TEST(MapInputToFreq, MapInputToFreqTest)
    {
        EXPECT_EQ(oa::derived_time::Frequency::kOnce, oa::enum_mappers::MapInputToFreq("ONCE"));
        EXPECT_EQ(oa::derived_time::Frequency::kAnnual, oa::enum_mappers::MapInputToFreq("ANNUAL"));
        EXPECT_EQ(oa::derived_time::Frequency::kSemiAnnual, oa::enum_mappers::MapInputToFreq("SEMIANNUAL"));
        EXPECT_EQ(oa::derived_time::Frequency::kQuarterly, oa::enum_mappers::MapInputToFreq("QUARTERLY"));
        EXPECT_EQ(oa::derived_time::Frequency::kMonthly, oa::enum_mappers::MapInputToFreq("MONTHLY"));
        EXPECT_EQ(oa::derived_time::Frequency::kWeekly, oa::enum_mappers::MapInputToFreq("WEEKLY"));
        EXPECT_EQ(oa::derived_time::Frequency::kDaily, oa::enum_mappers::MapInputToFreq("DAILY"));
    }

    TEST(MapInputToFreq, MapInputToFreqCaseInsensitiveTest)
    {
        EXPECT_EQ(oa::derived_time::Frequency::kOnce, oa::enum_mappers::MapInputToFreq("once"));
        EXPECT_EQ(oa::derived_time::Frequency::kAnnual, oa::enum_mappers::MapInputToFreq("annual"));
        EXPECT_EQ(oa::derived_time::Frequency::kSemiAnnual, oa::enum_mappers::MapInputToFreq("semiannual"));
        EXPECT_EQ(oa::derived_time::Frequency::kQuarterly, oa::enum_mappers::MapInputToFreq("quarterly"));
        EXPECT_EQ(oa::derived_time::Frequency::kMonthly, oa::enum_mappers::MapInputToFreq("monthly"));
        EXPECT_EQ(oa::derived_time::Frequency::kWeekly, oa::enum_mappers::MapInputToFreq("weekly"));
        EXPECT_EQ(oa::derived_time::Frequency::kDaily, oa::enum_mappers::MapInputToFreq("daily"));
    }

    TEST(MapInputToFreq, MapInputToFreqInvalidInputTest)
    {
        EXPECT_THROW(oa::enum_mappers::MapInputToFreq("INVALID"), std::invalid_argument);
    }


    TEST(MapInputToStub, MapInputToStubTest)
    {
        EXPECT_EQ(oa::derived_time::StubType::kNone, oa::enum_mappers::MapInputToStub("NONE"));
        EXPECT_EQ(oa::derived_time::StubType::kShortFirst, oa::enum_mappers::MapInputToStub("SHORTFIRST"));
        EXPECT_EQ(oa::derived_time::StubType::kLongFirst, oa::enum_mappers::MapInputToStub("LONGFIRST"));
        EXPECT_EQ(oa::derived_time::StubType::kShortLast, oa::enum_mappers::MapInputToStub("SHORTLAST"));
        EXPECT_EQ(oa::derived_time::StubType::kLongLast, oa::enum_mappers::MapInputToStub("LONGLAST"));
    }

    TEST(MapInputToStub, MapInputToStubCaseInsensitiveTest)
    {
        EXPECT_EQ(oa::derived_time::StubType::kNone, oa::enum_mappers::MapInputToStub("none"));
        EXPECT_EQ(oa::derived_time::StubType::kShortFirst, oa::enum_mappers::MapInputToStub("shortfirst"));
        EXPECT_EQ(oa::derived_time::StubType::kLongFirst, oa::enum_mappers::MapInputToStub("longfirst"));
        EXPECT_EQ(oa::derived_time::StubType::kShortLast, oa::enum_mappers::MapInputToStub("shortlast"));
        EXPECT_EQ(oa::derived_time::StubType::kLongLast, oa::enum_mappers::MapInputToStub("longlast"));
    }

    TEST(MapInputToStub, MapInputToStubInvalidInputTest)
    {
        EXPECT_THROW(oa::enum_mappers::MapInputToStub("INVALID"), std::invalid_argument);
    }

    TEST(MapInputToCalcType, MapInputToCalcTypeTest)
    {
        EXPECT_EQ(oa::derived_time::CalcType::kFlat, oa::enum_mappers::MapInputToCalcType("FLAT"));
        EXPECT_EQ(oa::derived_time::CalcType::kBBGCalcType1, oa::enum_mappers::MapInputToCalcType("BBG1"));
        EXPECT_EQ(oa::derived_time::CalcType::kBBGCalcType2, oa::enum_mappers::MapInputToCalcType("BBG2"));
        EXPECT_EQ(oa::derived_time::CalcType::kUSTStreetConv, oa::enum_mappers::MapInputToCalcType("USTSTREETCONV"));
    }

    TEST(MapInputToCalcType, MapInputToCalcTypeCaseInsensitiveTest)
    {
        EXPECT_EQ(oa::derived_time::CalcType::kFlat, oa::enum_mappers::MapInputToCalcType("flat"));
        EXPECT_EQ(oa::derived_time::CalcType::kBBGCalcType1, oa::enum_mappers::MapInputToCalcType("bbg1"));
        EXPECT_EQ(oa::derived_time::CalcType::kBBGCalcType2, oa::enum_mappers::MapInputToCalcType("bbg2"));
        EXPECT_EQ(oa::derived_time::CalcType::kUSTStreetConv, oa::enum_mappers::MapInputToCalcType("uststreetconv"));
    }

    TEST(MapInputToCalcType, MapInputToCalcTypeInvalidInputTest)
    {
        EXPECT_THROW(oa::enum_mappers::MapInputToCalcType("INVALID"), std::invalid_argument);
    }
    
}