

namespace oa::derived_time
{
	enum class Currency
	{
		kUSD,
		kEUR,
		kGBP
	};
	enum class CashflowType
	{
		kFixed,
		kFloating,
		kEquity
	};
	enum class DateDirection {
		kNone,
		kForward,
		kBackward
	};
} // namespace oa::derived_time