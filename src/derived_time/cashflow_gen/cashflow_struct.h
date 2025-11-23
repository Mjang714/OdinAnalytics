#ifndef OA_DERIVED_TIME_CAHFLOW_STRUCT_H_
#define OA_DERIVED_TIME_CAHFLOW_STRUCT_H_
#include <memory>
#include <string>

#include "oa/dllexport.h"
#include "time/date.h"
#include "derived_time/derived_time_enums.h"


namespace date = oa::time;

namespace oa::derived_time {
	struct OA_DERIVED_TIME_API CashflowStruct {
		date::Date start_date;
		date::Date end_date;
		date::Date fixing_date;
		date::Date payment_date;
		double notional;
		double rate;
		double cashflow_amount;
		double npv_cashflow_amount;
		int days;
		double day_count_fraction;
		Currency curr;
	};
}
#endif //OA_DERIVED_TIME_CAHFLOW_STRUCT_H_


