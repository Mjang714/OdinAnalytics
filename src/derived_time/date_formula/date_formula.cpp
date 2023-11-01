#include "date_formula.h"

#include "time/date_adjust/adjustment_factory.h"
#include "static_data_cache/calendar_cache.h"
#include "enum_mappers/time_enum_mappers.h"

namespace oa::derived_time {

DateFormula::DateFormula() :
	m_tenor_("0D"),
	m_holiday_adjuster_(oa::time::AdjustmentFactory::CreateDateAdjust(oa::time::AdjRule::kPlainAdjustment))
{

}
DateFormula::DateFormula(const oa::time::Tenor& tenor_obj, oa::time::AdjRule adjustment_rule, const oa::time::Calendar& calendar) :
	m_tenor_(tenor_obj),
	m_holiday_adjuster_(oa::time::AdjustmentFactory::CreateDateAdjust(adjustment_rule, calendar))
{

}

DateFormula::DateFormula(const std::string& tenor_obj, oa::time::AdjRule adjustment_rule, const oa::time::Calendar& calendar) :
	m_tenor_(tenor_obj),
	m_holiday_adjuster_(oa::time::AdjustmentFactory::CreateDateAdjust(adjustment_rule, calendar))
{

}

DateFormula::DateFormula(const std::string& tenor_obj, oa::time::AdjRule adjustment_rule, const std::string& calendar) :
	m_tenor_(tenor_obj),
	m_holiday_adjuster_(oa::time::AdjustmentFactory::CreateDateAdjust(adjustment_rule,
	*oa::static_cache::CalendarCache::RetrieveCache().GetCalendar(calendar)))
{

}

DateFormula::DateFormula(const std::string& tenor_obj, const std::string& adjustment_rule, const std::string& calendar) :
	m_tenor_(tenor_obj),
	m_holiday_adjuster_(oa::time::AdjustmentFactory::CreateDateAdjust(oa::enum_mappers::MapInputToDayAdjustEnum(adjustment_rule),
	*oa::static_cache::CalendarCache::RetrieveCache().GetCalendar(calendar)))
{

}

oa::time::Date DateFormula::Adjust(const oa::time::Date& date_obj) const
{
	auto result_date(date_obj);
	return  m_holiday_adjuster_->AdjustDate(result_date.AddTenor(m_tenor_));
}
}