#include "cashflow_gen.h"


namespace oa::derived_time {


	std::vector<CashflowStruct> CashflowGen::CreateCashflows(
		const oa::time::Date& start_date,
		const oa::time::Date& mat_date,
		const oa::derived_time::Frequency reset_freq,
		const double notional,
		const double rate,
		const oa::time::DayCountRule day_count_rule,
		const oa::derived_time::DateDirection date_dir,
		const oa::derived_time::CashflowType cf_type,
		const oa::derived_time::ResetDirection rest_dir,
		const oa::derived_time::StubType& stub_type,
		const std::shared_ptr<oa::derived_time::DateFormula>& payment_date_adj,
		const std::shared_ptr<oa::derived_time::DateFormula>& fixing_date_adj,
		const std::shared_ptr<oa::derived_time::DateFormula>& acc_date_adj,
		const std::optional<oa::time::Date>& stub_date
	) {
		std::vector<CashflowStruct> cashflows{};

		std::vector<oa::time::Date> unadjusted_start_dates{};
		std::vector<oa::time::Date> unadjusted_end_dates{};

		//this is not a good palce to place this but I need to think about where else I can put this mapping.
		const static std::unordered_map<oa::derived_time::Frequency, oa::time::Tenor> reset_freq_enum_to_tenor{
			{oa::derived_time::Frequency::kAnnual, oa::time::Tenor("1Y")},
			{oa::derived_time::Frequency::kSemiAnnual,oa::time::Tenor("6M")},
			{oa::derived_time::Frequency::kQuarterly, oa::time::Tenor("3M")},
			{oa::derived_time::Frequency::kMonthly, oa::time::Tenor("1M")},
			{oa::derived_time::Frequency::kWeekly, oa::time::Tenor("1W")},
			{oa::derived_time::Frequency::kDaily, oa::time::Tenor("1D")}
		};

		auto tenor_pair = reset_freq_enum_to_tenor.at(reset_freq).GetValues();
		auto time_length = tenor_pair.first;
		auto tenor_enum = tenor_pair.second;

		if (date_dir == deriv_time::DateDirection::kForward) {
			auto curr_start_date = start_date;
			auto total_length = time_length;
			while (curr_start_date < mat_date) {

				if (curr_start_date < mat_date) {
					unadjusted_start_dates.emplace_back(curr_start_date);
					unadjusted_end_dates.emplace_back(curr_start_date.AddTenor(oa::time::Tenor(time_length, tenor_enum)));
				}
				curr_start_date = start_date.AddTenor(oa::time::Tenor(total_length, tenor_enum));
				total_length += time_length;
			}
		}

		else if (date_dir == deriv_time::DateDirection::kBackward) {
			
			auto curr_end_date = mat_date;
			auto total_length = -time_length;
			while (curr_end_date > start_date) {
				if (curr_end_date > start_date) {
					unadjusted_end_dates.emplace_back(curr_end_date);
					unadjusted_start_dates.emplace_back(curr_end_date.AddTenor(oa::time::Tenor(-time_length, tenor_enum)));
				}
				curr_end_date = mat_date.AddTenor(oa::time::Tenor(total_length, tenor_enum));
				total_length -= time_length;
			}
		}

		else {
			// No date direction specified
		}

		if (stub_type == deriv_time::StubType::kShortFirst) {
			//will fill out later to deal with short first stub
		}
		else if (stub_type == deriv_time::StubType::kShortFirst) {
			//will fill out later to deal with short last stub

		}

		for (size_t i = 0; i < unadjusted_start_dates.size(); i++) {
			auto day_count = oa::time::DayCounterFactory::GenerateDayCounter(day_count_rule);
			CashflowStruct cf{};
			cf.start_date = unadjusted_start_dates[i];
			cf.end_date = unadjusted_end_dates[i];
			cf.notional = notional;
			cf.rate = rate;
			cf.cashflow_amount = notional * (rate / oa::utils::EnumToInt<oa::derived_time::Frequency>(reset_freq));
			cf.days = day_count->DayCount(cf.start_date, cf.end_date);
			cf.day_count_fraction = day_count->YearFraction(cf.start_date, cf.end_date);
			cf.cf_type = cf_type;
			if (payment_date_adj != nullptr) {
				cf.payment_date = payment_date_adj->Adjust(cf.end_date);
			}
			else {
				cf.payment_date = cf.end_date;
			}

			if (fixing_date_adj != nullptr) {
				if(rest_dir == oa::derived_time::ResetDirection::kForward) {
					cf.fixing_date = fixing_date_adj->Adjust(cf.start_date);
				}
				else {
					cf.fixing_date = fixing_date_adj->Adjust(cf.end_date);
				}
			} 
			else {
				cf.fixing_date = cf.start_date;
			}
			// using emplace_back and std::move though not sure if it is necessary here
			cashflows.emplace_back(std::move(cf));
		}
		// Implementation logic to generate cashflows goes here
		return cashflows;

	}
}  // namespace oa::derived_time