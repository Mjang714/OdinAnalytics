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
		date::Date unadj_start_date;
		date::Date unadj_end_date;
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
		Currency cf_curr;
		CashflowType cf_type;

		CashflowStruct() = default;	

		CashflowStruct(
			std::string unadj_start_str,
			std::string unadj_end_str,
			std::string start_str,
			std::string end_str,
			std::string fixing_str,
			std::string payment_str,
			double inpt_notional,
			double input_rate,
			double input_cashflow_amount,
			double input_npv_cashflow_amount,
			int input_days,
			double input_day_count_fraction,
			Currency input_cf_curr,
			CashflowType input_cf_type) :
			unadj_start_date(unadj_start_str),
			unadj_end_date(unadj_end_str),
			start_date(start_str), 
			end_date(end_str),
			fixing_date(fixing_str),
			payment_date(payment_str),
			notional(inpt_notional),
			rate(input_rate),
			cashflow_amount(input_cashflow_amount),
			npv_cashflow_amount(input_npv_cashflow_amount),
			days(input_days),
			day_count_fraction(input_day_count_fraction),
			cf_curr(input_cf_curr),
			cf_type(input_cf_type)
			{

			}

		bool operator==(const CashflowStruct& right_value) const {
			//not sure if std::tied is the best way to do this but it seems to work
			return 
				std::tie(
				unadj_start_date,
				unadj_end_date,
				start_date,
				end_date,
				fixing_date,
				payment_date,
				notional,
				rate,
				cashflow_amount,
				npv_cashflow_amount,
				days,
				day_count_fraction,
				cf_curr,
				cf_type) 

				==

				std::tie(
					right_value.unadj_start_date,
					right_value.unadj_end_date,
					right_value.start_date,
					right_value.end_date,
					right_value.fixing_date,
					right_value.payment_date,
					right_value.notional,
					right_value.rate,
					right_value.cashflow_amount,
					right_value.npv_cashflow_amount,
					right_value.days,
					right_value.day_count_fraction,
					right_value.cf_curr,
					right_value.cf_type);
		}
	};

	
}
#endif //OA_DERIVED_TIME_CAHFLOW_STRUCT_H_


