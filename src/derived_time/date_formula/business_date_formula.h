#ifndef OA_DERIVED_TIME_DATE_FORMULA_SPOT_DATE_FORMULA_H_
#define OA_DERIVED_TIME_DATE_FORMULA_SPOT_DATE_FORMULA_H_

#include "oa/dllexport.h"
#include "time/calendar.h"



namespace oa::derived_time
{
	class OA_DERIVED_TIME_API BusinessDateFormula
	{
		public: 
			BusinessDateFormula() = default;
			BusinessDateFormula(int business_days, const std::shared_ptr<const oa::time::Calendar>& calendar_input);
			BusinessDateFormula(int business_days, const std::string& calendars);
			oa::time::Date Adjust(const oa::time::Date& base_date) const;

		private:
			std::shared_ptr<const oa::time::Calendar> m_calendar_{};
			int m_num_of_business_days{ 0 };
	};

}
#endif // !OA_DERIVED_TIME_DATE_FORMULA_SPOT_DATE_FORMULA_H_


