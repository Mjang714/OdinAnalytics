

namespace oa::derived_time
{
	enum class Currency
	{
		kUSD,
		kEUR,
		kGBP,
		kJPY,
		kCHF,
		kAUD,
		kCAD,
		kNZD,
		kCNY,
		kINR
	};

	enum class CashflowType
	{
		kFixed,
		kFloating,
		kPrincipal,
		kEquity
	};

	enum class DateDirection {
		kNone,
		kForward,
		kBackward
	};

	enum class StubType {
		kNone,
		kShortFirst,
		kShortLast,
		kLongFirst,
		kLongLast
	};

	enum class kFrequency
	{
		kOnce = 0,
		kAnnual = 1,
		kSemiAnnual = 2,
		kQuarterly = 3,
		kMonthly = 12,
		kWeekly = 52,
		kDaily = 365
	};
} // namespace oa::derived_time