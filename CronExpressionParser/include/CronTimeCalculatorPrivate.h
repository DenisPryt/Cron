#pragma once

#include <chrono>
#include <date/date.h>

namespace Cron
{

struct SheduleMasks;

class TimeCalculator
{
	using seconds = std::chrono::seconds;
	using minutes = std::chrono::minutes;
	using hours = std::chrono::hours;

	using sys_time_t = std::chrono::system_clock::time_point;
	using sys_duration = std::chrono::system_clock::duration;

	using year_month_day = date::year_month_day;
	using sys_days = date::sys_days;

	using time_of_day_sys = date::time_of_day<sys_duration>;
	using time_of_day_sec = date::time_of_day<seconds>;

public:
	explicit TimeCalculator(SheduleMasks& sheduleMasksRef);

	sys_time_t Next(const sys_time_t& from);
	sys_time_t Prev(const sys_time_t& from);

	
	time_of_day_sec NextTimeOfDay(time_of_day_sec tod);
    year_month_day NextMonth(year_month_day ymd);
    
    time_of_day_sec PrevTimeOfDay(time_of_day_sec tod);
    year_month_day PrevMonth(year_month_day ymd);
	
    year_month_day AddDay(year_month_day ymd, date::days d);

	
	bool IsDayOfWeekAllowed(const sys_days& dayPoint);
	bool IsDayOfMonthAllowed(const year_month_day& ymd);
	bool IsDayOfMonthAllowed(const sys_days& dayPoint);

private:
	SheduleMasks& m_masks;
};

}  // namespace Cron

