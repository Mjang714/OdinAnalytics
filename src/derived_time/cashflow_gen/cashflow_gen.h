#ifndef OA_DERIVED_TIME_CASHFLOW_GEN_CASHFLOW_GEN_H_
#define OA_DERIVED_TIME_CASHFLOW_GEN_CASHFLOW_GEN_H_

#include <memory>
#include <string>

#include "oa/dllexport.h"
#include "derived_time/derived_time_enums.h"
#include "derived_time/cashflow_gen/cashflow_struct.h"
#include "derived_time/date_formula/date_formula.h"
#include "time/calendar.h"
#include "time/date.h"
#include "time/tenor.h"
#include "time/date_adjust/date_adjust_base.h"
#include "time/day_count/day_counter_factory.h"
#include "helpers/utils.h"


namespace deriv_time = oa::derived_time;
namespace oa::derived_time {
	class OA_DERIVED_TIME_API CashflowGen {
	public:
		CashflowGen() = default;
		static std::vector<CashflowStruct> CreateCashflows(
			const oa::time::Date& start_date,
			const oa::time::Date& mat_date,
			const oa::derived_time::Frequency reset_freq,
			const double notional,
			const double rate,
			const oa::time::DayCountRule day_count_rule,
			const oa::derived_time::Currency cf_curr = oa::derived_time::Currency::kUSD,
			const oa::derived_time::DateDirection date_dir = deriv_time::DateDirection::kBackward,
			const oa::derived_time::CashflowType cf_type = oa::derived_time::CashflowType::kFixed,
			const oa::derived_time::ResetDirection rest_dir = oa::derived_time::ResetDirection::kAdvance,
			const oa::derived_time::StubType& stub_type = deriv_time::StubType::kNone,
			const std::shared_ptr<oa::derived_time::DateFormula>& start_date_adj = nullptr,
			const std::shared_ptr<oa::derived_time::DateFormula>& end_date_adj = nullptr,
			const std::shared_ptr<oa::derived_time::DateFormula>& payment_date_adj = nullptr,
			const std::shared_ptr<oa::derived_time::DateFormula>& fixing_date_adj = nullptr,
			const std::shared_ptr<oa::derived_time::DateFormula>& acc_date_adj = nullptr,
			const std::optional<oa::time::Date>& stub_date = std::nullopt
			);
	

	};
}
#endif // !OA_DERIVED_TIME_CASHFLOW_GEN_CASHFLOW_GEN_H_
