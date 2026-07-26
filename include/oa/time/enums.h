#ifndef OA_TIME_ENUMS_H_
#define OA_TIME_ENUMS_H_

#include "oa/dllexport.h"

#include <iosfwd>

namespace oa::time
{
	enum class Weekdays
	{
		kMonday,
		kTuesday,
		kWednesday,
		kThursday,
		kFriday,
		kSaturday,
		kSunday,
	};

	enum class Months
	{
		kJanuary = 1,
		kFebruary = 2,
		kMarch = 3,
		kApril = 4,
		kMay = 5,
		kJune = 6,
		kJuly = 7,
		kAugust = 8,
		kSeptember = 9,
		kOctober = 10,
		kNovember = 11,
		kDecember = 12
	};

	enum class Tenors
	{
		kDays = 'D',
		kWeeks = 'W',
		kMonths = 'M',
		kYears = 'Y'
	};

	enum class AdjRule
	{
		kFollowing,
		kPreceding,
		kModifiedFollowing,
		kPlainAdjustment
	};

	enum class DayCountRule
	{
		kACT_ACT,
		kACTB_ACTB,
		k30_360_BOND_BASIS,
		k30_E_360_ISDA,
		k30_360_E_EUROBOND,
		kACT_360,
		kACT_365_FIXED

	};

    OA_TIME_API
    std::ostream& operator<<(std::ostream& os, DayCountRule val);

    OA_TIME_API
    std::ostream& operator<<(std::ostream& os, AdjRule val);
}

#endif  // OA_TIME_ENUMS_H_
