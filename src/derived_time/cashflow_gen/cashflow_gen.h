#ifndef OA_DERIVED_TIME_CAHFLOW_GEN_H_
#define OA_DERIVED_TIME_CAHFLOW_GEN_H_

#include <memory>
#include <string>

#include "oa/dllexport.h"
#include "derived_time/derived_time_enums.h"
#include "time/calendar.h"
#include "time/tenor.h"
#include "time/date_adjust/date_adjust_base.h"

namespace oa::derived_time {
	class OA_DERIVED_TIME_API CashflowGen {
		static std::vector<CashflowGen> CreateCashflowGen(
			const oa::time::Calendar& calendar,
			const oa::time::Date& start_date,
			const oa::time::Date& mat_date,
			const oa::time::Tenor& frequency,
			const oa::time::AdjRule& pay_adj_rule,
			const oa::derived_time::DateDirection& date_direction,
			const oa::derived_time::StubType& stub_type);

	};
}
#endif // !OA_DERIVED_TIME_CAHFLOW_GEN_H_
