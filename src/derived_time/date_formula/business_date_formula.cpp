#include "business_date_formula.h"
#include "static_data_cache/calendar_cache.h"



namespace oa::derived_time
{
	BusinessDateFormula::BusinessDateFormula(int business_days, const std::shared_ptr<const oa::time::Calendar>& input_calendar) :
		m_num_of_business_days(business_days) ,
		m_calendar_(input_calendar) 
	{
	}

	BusinessDateFormula::BusinessDateFormula(int business_days, const std::string& calendar_str) :
		m_num_of_business_days(business_days),
		m_calendar_(oa::static_cache::CalendarCache::RetrieveCache().GetCalendar(calendar_str))
	{

	}

	oa::time::Date BusinessDateFormula::Adjust(const oa::time::Date& base_date) const
	{
		return m_calendar_->AddBusinessDays(m_num_of_business_days, base_date);;
	}
}