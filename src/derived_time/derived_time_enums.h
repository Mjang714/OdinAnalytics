#ifndef OA__DERIVED_TIME_DERIVED_TIME_ENUMS_H_
#define OA__DERIVED_TIME_DERIVED_TIME_ENUMS_H_

#include "oa/dllexport.h"

#include <iostream>
#include <string>

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

	enum class ResetDirection {
		kAdvance,
		kInArears
	};

	enum class Frequency
	{
		kOnce = 0,
		kAnnual = 1,
		kSemiAnnual = 2,
		kQuarterly = 3,
		kMonthly = 12,
		kWeekly = 52,
		kDaily = 365
	};

	enum class CalcType
	{
		kFlat,
		kCompounding,
		kBBGCalcType1,
		kUSTStreetConv,
		kBBGCalcType2
	};

	OA_DERIVED_TIME_API
	derived_time::DateDirection MapInputToDateDir(const std::string& input_str);

	OA_DERIVED_TIME_API
	derived_time::Frequency MapInputToFreq(const std::string& input_str);

	OA_DERIVED_TIME_API
	derived_time::StubType MapInputToStub(const std::string& input_str);

	OA_DERIVED_TIME_API
	derived_time::CalcType MapInputToCalcType(const std::string& input_str);
	
	OA_DERIVED_TIME_API
	derived_time::DateDirection MapInputToDateDir(const std::string& input_str);

	OA_DERIVED_TIME_API
	derived_time::Frequency MapInputToFreq(const std::string& input_str);

	OA_DERIVED_TIME_API
	derived_time::StubType MapInputToStub(const std::string& input_str);

	OA_DERIVED_TIME_API
	derived_time::CalcType MapInputToCalcType(const std::string& input_str);

	OA_DERIVED_TIME_API
	std::ostream& operator<<(std::ostream& os, DateDirection val);

	OA_DERIVED_TIME_API
	std::ostream& operator<<(std::ostream& os, Frequency val);

	OA_DERIVED_TIME_API
	std::ostream& operator<<(std::ostream& os, StubType val);

	OA_DERIVED_TIME_API
	std::ostream& operator<<(std::ostream& os, CalcType val);
}


#endif // !OA__DERIVED_TIME_DERIVED_TIME_ENUMS_H_

