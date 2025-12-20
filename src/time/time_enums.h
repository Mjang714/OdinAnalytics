#ifndef ODINANALYTICS_TIME_TIMEENUMS_H
#define ODINANALYTICS_TIME_TIMEENUMS_H

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
	
}

#endif // ODINANALYTICS_TIME_TIMEENUMS_H

