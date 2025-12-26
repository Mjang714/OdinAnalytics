#ifndef OA_DERIVED_TIME_DATE_FORMULA_SPOT_DATE_FORMULA_H_
#define OA_DERIVED_TIME_DATE_FORMULA_SPOT_DATE_FORMULA_H_

#include "oa/dllexport.h"
#include "time/calendar.h"
#include "derived_time/date_formula/date_formula.h"


namespace oa::derived_time
{
	class OA_DERIVED_TIME_API BusinessDateFormula
	{
		public: 
			BusinessDateFormula() = default;
			BusinessDateFormula(int business_days, const std::shared_ptr<const oa::time::Calendar>& calendar_input);
			BusinessDateFormula(int business_days, const std::string& calendars);
			oa::time::Date Adjust(const oa::time::Date& base_date) const;
			friend oa::time::Date operator+(const oa::time::Date& base_date, const BusinessDateFormula& bus_date_formula);
			friend oa::time::Date operator+(const BusinessDateFormula& bus_date_formula, const oa::time::Date& base_date);

		private:
			std::shared_ptr<const oa::time::Calendar> m_calendar_{};
			int m_num_of_business_days{ 0 };
	};

	oa::time::Date operator+(const oa::time::Date& base_date, const BusinessDateFormula& bus_date_formula) {
		return bus_date_formula.Adjust(base_date);
	}

	oa::time::Date operator+(const BusinessDateFormula& bus_date_formula, const oa::time::Date& base_date) {
		return bus_date_formula.Adjust(base_date);
	}


	oa::time::Date operator+(const oa::time::Date& date_obj, const oa::derived_time::DateFormula& date_formula_rule) {
		return date_formula_rule.Adjust(date_obj);
	}

	oa::time::Date operator+(const oa::derived_time::DateFormula& date_formula_rule, const oa::time::Date& date_obj) {
		return date_formula_rule.Adjust(date_obj);
	}
}
#endif // !OA_DERIVED_TIME_DATE_FORMULA_SPOT_DATE_FORMULA_H_


