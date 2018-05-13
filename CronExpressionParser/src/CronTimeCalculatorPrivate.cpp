#include "CronTimeCalculatorPrivate.h"
#include "CronSheduleMasks.h"


using namespace std::chrono_literals;
using namespace date;


namespace Cron
{


TimeCalculator::TimeCalculator(SheduleMasks& sheduleMasksRef)
	: m_masks{sheduleMasksRef}
{}


auto TimeCalculator::Next(const sys_time_t& from) -> sys_time_t
{
	if (m_masks.IsEmpty())
		return {};

	const sys_days nowDays = floor<days>(from);

	sys_duration addTime = NextTimeOfDay(make_time(floor<seconds>(from - nowDays + 1s))).to_duration();

	auto ymd = year_month_day{floor<days>(nowDays + addTime)};

	while (!m_masks.Months[(unsigned)ymd.month()] ||
		   !(IsDayOfWeekAllowed(sys_days{ymd}) && IsDayOfMonthAllowed(ymd)))
	{
		ymd = NextMonth(ymd);
        ymd = AddDay(ymd, date::days{+1});

		if (sys_days{ymd} - from > years{5})
			return {};
	}

	if (nowDays != sys_days{ymd})
		addTime = NextTimeOfDay(make_time(0s)).to_duration();

	return sys_days{ymd} + addTime;
}


auto TimeCalculator::Prev(const sys_time_t& from) -> sys_time_t
{
    if (m_masks.IsEmpty())
        return {};
    
    const sys_days nowDays = floor<days>(from);
    
    sys_duration subTime = PrevTimeOfDay(make_time(floor<seconds>(from - nowDays - 1s))).to_duration();
    
    auto ymd = year_month_day{floor<days>(nowDays - subTime)};
    
    while (!m_masks.Months[(unsigned)ymd.month()] ||
           !(IsDayOfWeekAllowed(sys_days{ymd}) && IsDayOfMonthAllowed(ymd)))
    {
        ymd = PrevMonth(ymd);
        ymd = AddDay(ymd, date::days{-1});
        
        if (from - sys_days{ymd} > years{5})
            return {};
    }
    
    if (nowDays != sys_days{ymd})
        subTime = PrevTimeOfDay(make_time(24h - 1s)).to_duration();
        
    return sys_days{ymd} - subTime;
}


auto TimeCalculator::NextTimeOfDay(time_of_day_sec tod) -> time_of_day_sec
{
	while (!m_masks.Seconds[(size_t)tod.seconds().count()] ||
		   !m_masks.Minutes[(size_t)tod.minutes().count()] ||
		   !m_masks.Hours[tod.hours().count() % 24])
	{
		while (!m_masks.Seconds[(size_t)tod.seconds().count()])
		{
			tod = make_time(tod.to_duration() + 1s);
		}

		while (!m_masks.Minutes[tod.minutes().count()])
		{
			auto nextMin = tod.minutes() < 59min ? tod.minutes() + 1min : 0min;
			auto nextHour = tod.minutes() < 59min ? tod.hours() : tod.hours() + 1h;
			tod = make_time(nextHour, nextMin, 0s, 0);
		}

		while (!m_masks.Hours[tod.hours().count() % 24])
		{
			tod = make_time(tod.hours() + 1h, 0min, 0s, 0);
		}
	}

	return tod;
}


auto TimeCalculator::NextMonth(year_month_day ymd) -> year_month_day
{
	while (!m_masks.Months[(unsigned)ymd.month()])
	{
		ymd += months{1};

		if (!ymd.ok())
			ymd = ymd.year() / ymd.month() / 1;
	}

	return ymd;
}

    
auto TimeCalculator::PrevTimeOfDay(time_of_day_sec tod) -> time_of_day_sec
{
    while (!m_masks.Seconds[(size_t)tod.seconds().count()] ||
           !m_masks.Minutes[(size_t)tod.minutes().count()] ||
           !m_masks.Hours[tod.hours().count() % 24])
    {
        while (!m_masks.Seconds[(size_t)tod.seconds().count()])
        {
            tod = make_time(tod.to_duration() - 1s);
        }
            
        while (!m_masks.Minutes[tod.minutes().count()])
        {
            auto prevMin = tod.minutes() > 0min ? tod.minutes() - 1min : 59min;
            auto prevHour = tod.minutes() > 0min ? tod.hours() : tod.hours() - 1h;
            tod = make_time(prevHour, prevMin, 0s, 0);
        }
        
        while (!m_masks.Hours[tod.hours().count() % 24])
        {
            tod = make_time(tod.hours() - 1h, 0min, 0s, 0);
        }
    }
        
    return tod;
}


auto TimeCalculator::PrevMonth(year_month_day ymd) -> year_month_day
{
    while (!m_masks.Months[(unsigned)ymd.month()])
    {
        ymd -= months{1};
        
        if (!ymd.ok())
            ymd = ymd.year() / ymd.month() / last;
    }
    
    return ymd;
}
    

auto TimeCalculator::AddDay(year_month_day ymd, date::days d) -> year_month_day
{
	auto dayPoint = sys_days(ymd);

	while (!(IsDayOfWeekAllowed(dayPoint) && IsDayOfMonthAllowed(dayPoint)))
	{
		dayPoint += d;
	}

	return dayPoint;
}


bool TimeCalculator::IsDayOfWeekAllowed(const sys_days& dayPoint)
{
	year_month_weekday ymw{dayPoint};
	year_month_weekday_last ymwl = ymw.year() / ymw.month() / ymw.weekday()[last];

	return m_masks.DaysOfWeek[(unsigned)ymw.weekday()] ||
		(m_masks.DaysOfWeekLast[(unsigned)ymwl.weekday()] && sys_days{ymwl} == dayPoint) ||
		m_masks.DaysOfWeekIndex[(unsigned)ymwl.weekday() + (7u * (ymw.index() - 1u))];
}


bool TimeCalculator::IsDayOfMonthAllowed(const year_month_day& ymd)
{
	return m_masks.DaysOfMonth[(unsigned)ymd.day()] ||
		(m_masks.DaysOfMonth[SheduleMasks::LastDayOfMonthBit] && ymd == (ymd.year() / ymd.month() / last));
}


bool TimeCalculator::IsDayOfMonthAllowed(const sys_days& dayPoint)
{
	return IsDayOfMonthAllowed(year_month_day{dayPoint});
}

}  // namespace Cron
