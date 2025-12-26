#ifndef OA_DERIVED_TIME_DATE_FORMULA_H_
#define OA_DERIVED_TIME_DATE_FORMULA_H_

#include <memory>
#include <string>

#include "oa/dllexport.h"
#include "time/calendar.h"
#include "time/tenor.h"
#include "time/date_adjust/date_adjust_base.h"

namespace oa::derived_time {
class OA_DERIVED_TIME_API DateFormula {

	public:
		DateFormula();
		DateFormula(const oa::time::Tenor& tenor_obj, oa::time::AdjRule adjustment_rule, const oa::time::Calendar& calendar);
		DateFormula(const std::string& tenor_obj,  oa::time::AdjRule adjustment_rule, const oa::time::Calendar& calendar);
		DateFormula(const std::string& tenor_obj, oa::time::AdjRule adjustment_rule, const std::string& calendar);
		DateFormula(const std::string& tenor_obj, const std::string& adjustment_rule, const std::string& calendar);
			
		/// <summary>
		/// Applies a tenor addtion and date adjustment to a date this allows us to apply date logic
		/// </summary>
		/// <param name="date_obj"></param>
		/// <returns></returns>
		oa::time::Date Adjust(const oa::time::Date& date_obj) const;

		friend oa::time::Date operator+(const oa::time::Date& date_obj, const oa::derived_time::DateFormula& date_formula_rule);
		friend oa::time::Date operator+(const oa::derived_time::DateFormula& date_formula_rule, const oa::time::Date& date_obj);
	private:
		oa::time::Tenor m_tenor_;
		std::unique_ptr<oa::time::DateAdjustBase> m_holiday_adjuster_;
};

	oa::time::Date operator+(const oa::time::Date& date_obj, const oa::derived_time::DateFormula& date_formula_rule) {
		return date_formula_rule.Adjust(date_obj);
	}

	oa::time::Date operator+(const oa::derived_time::DateFormula& date_formula_rule, const oa::time::Date& date_obj) {
		return date_formula_rule.Adjust(date_obj);
	}
}

#endif //!OA_DERIVED_TIME_DATE_FORMULA_H_