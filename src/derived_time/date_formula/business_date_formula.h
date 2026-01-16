#ifndef OA_DERIVED_TIME_DATE_FORMULA_SPOT_DATE_FORMULA_H_
#define OA_DERIVED_TIME_DATE_FORMULA_SPOT_DATE_FORMULA_H_

#include <memory>
#include <string>

#include "oa/dllexport.h"
#include "time/calendar.h"
#include "time/date.h"

namespace oa::derived_time
{
	class OA_DERIVED_TIME_API BusinessDateFormula
	{
		public:
			/**
			 * Default ctor.
			 *
			 * This constructs an adjustment of zero days without a calendar.
			 */
			BusinessDateFormula() = default;

			BusinessDateFormula(int business_days, const std::shared_ptr<const time::Calendar>& calendar_input);
			BusinessDateFormula(int business_days, const std::string& calendars);

			// TODO: document more
			// updated so that if business day count is zero no adjustment is done.
			// this allows BDFs without a calendar to work
			time::Date Adjust(const time::Date& base_date) const;

			// number of adjustment days
			int days() const noexcept;

			// calendar
			// TODO: doesn't need to be const Calendar in the shared_ptr
			const time::Calendar* calendar() const noexcept;

			// indicate that adjustment is nonzero
			operator bool() const noexcept;

		private:
			int m_num_of_business_days{ 0 };
			std::shared_ptr<const time::Calendar> m_calendar_{};
	};

	inline time::Date operator+(const time::Date& base_date, const BusinessDateFormula& bus_date_formula) {
		return bus_date_formula.Adjust(base_date);
	}

	inline time::Date operator+(const BusinessDateFormula& bus_date_formula, const time::Date& base_date) {
		return bus_date_formula.Adjust(base_date);
	}

}
#endif // !OA_DERIVED_TIME_DATE_FORMULA_SPOT_DATE_FORMULA_H_
