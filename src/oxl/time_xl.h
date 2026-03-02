#ifndef OXL_TIME_XL_H_
#define OXL_TIME_XL_H_

#include "xl_api/xl_dictionary.h"

// forward decl to avoid bringing in XLCALL.H
struct xloper12;

namespace oxl
{
	bool OxlIsBizDay(const xloper12* date, const xloper12* centers);
	bool OxlIsHolDay(const xloper12* date, const xloper12* centers);

	int OxlComputeDayCount(
		const xloper12* start_date,
		const xloper12* end_date,
		const xloper12* busines_day_count);

	double OxlComputeYearFraction(
		const xloper12* start_date,
		const xloper12* end_date,
		const xloper12* busines_day_count);

	double OxlComputeDate(const xl_api::XlDictionary& dictionary);
	double OxlAddBusinessDays(const xl_api::XlDictionary& dictionary);
}

#endif //OXL_TIME_XL_H_
