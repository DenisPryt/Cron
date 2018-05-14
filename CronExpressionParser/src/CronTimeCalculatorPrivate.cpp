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

	sys_duration addTime = NextTimeOfDay(make_time(floor<seconds>(from - nowDays + 1s)));

	auto ymd = year_month_day{floor<days>(nowDays + addTime)};

	if (!IsYearDayMonthAllowed(ymd))
	{
		do
		{
			ymd = NextMonth(ymd);
			ymd = AddDay(ymd, days{+1});

			if (sys_days{ymd} - from > years{5})
				return {};

		} while (!IsYearDayMonthAllowed(ymd));

		return sys_days{ymd} + NextTimeOfDay(make_time(0s));
	}

	return nowDays + addTime;
}


auto TimeCalculator::Prev(const sys_time_t& from) -> sys_time_t
{
    if (m_masks.IsEmpty())
        return {};
    
    const sys_days nowDays = floor<days>(from);
    
    sys_duration addTime = PrevTimeOfDay(make_time(floor<seconds>(from - nowDays - 1s)));
    
    auto ymd = year_month_day{floor<days>(nowDays + addTime)};
    
	if (!IsYearDayMonthAllowed(ymd))
	{
		do
		{
			ymd = PrevMonth(ymd);
			ymd = AddDay(ymd, days{-1});

			if (from - sys_days{ymd} > years{5})
				return {};

		} while (!IsYearDayMonthAllowed(ymd));

		return sys_days{ymd} + PrevTimeOfDay(make_time(24h - 1s));
	}
        
    return nowDays + addTime;
}


auto TimeCalculator::NextTimeOfDay(time_of_day_sec tod) -> seconds
{
	while (!IsTimeOfDayAllowed(tod))
	{
		while (!m_masks.Seconds[(size_t)tod.seconds().count()])
		{
			tod = make_time(tod.to_duration() + 1s);
		}

		while (!m_masks.Minutes[tod.minutes().count()])
		{
			auto nextMin = tod.minutes() < 59min ? tod.minutes() + 1min : 0min;
			auto nextHour = tod.minutes() < 59min ? tod.hours() : tod.hours() + 1h;

			//if (nextHour > 48h)
			//	throw std::runtime_error("Next time calculate loop");

			tod = make_time(nextHour, nextMin, 0s, 0);
		}

		while (!m_masks.Hours[tod.hours().count() % 24])
		{
			//if (tod.hours() >= 48h)
			//	throw std::runtime_error("Next time calculate loop");

			tod = make_time(tod.hours() + 1h, 0min, 0s, 0);
		}
	}

	return tod.to_duration();
}


auto TimeCalculator::NextMonth(year_month_day ymd) -> year_month_day
{
	while (!m_masks.Months[(unsigned)ymd.month()])
	{
		ymd += months{1};
		ymd = ymd.year() / ymd.month() / 1;
	}

	return ymd;
}

    
auto TimeCalculator::PrevTimeOfDay(time_of_day_sec tod) -> seconds
{
	tod = make_time(tod.to_duration() + 24h);
	
	while (!IsTimeOfDayAllowed(tod))
    {
        while (!m_masks.Seconds[(size_t)tod.seconds().count()])
        {
            tod = make_time(tod.to_duration() - 1s);
        }
            
        while (!m_masks.Minutes[tod.minutes().count()])
        {
            auto prevMin = tod.minutes() > 0min ? tod.minutes() - 1min : 59min;
            auto prevHour = tod.minutes() > 0min ? tod.hours() : tod.hours() - 1h;

			//if (prevHour < 0h)
			//	throw std::runtime_error("Previous time calculate loop");

            tod = make_time(prevHour, prevMin, 59s, 0);
        }
        
        while (!m_masks.Hours[tod.hours().count() % 24])
        {
			//if (tod.hours() <= 0h)
			//	throw std::runtime_error("Previous time calculate loop");

			tod = make_time(tod.hours() - 1h, 59min, 59s, 0);
        }
    }
        
    return tod.to_duration() - 24h;
}


auto TimeCalculator::PrevMonth(year_month_day ymd) -> year_month_day
{
    while (!m_masks.Months[(unsigned)ymd.month()])
    {
        ymd -= months{1};
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


bool TimeCalculator::IsTimeOfDayAllowed(const time_of_day_sec& tod) const
{
	return m_masks.Seconds[(size_t)tod.seconds().count()] &&
		m_masks.Minutes[(size_t)tod.minutes().count()] &&
		m_masks.Hours[tod.hours().count() % 24];
}


bool TimeCalculator::IsYearDayMonthAllowed(const year_month_day& ymd) const
{
	return m_masks.Months[(unsigned)ymd.month()] &&
		(IsDayOfWeekAllowed(sys_days{ymd}) && IsDayOfMonthAllowed(ymd));
}


bool TimeCalculator::IsDayOfWeekAllowed(const sys_days& dayPoint) const
{
	year_month_weekday ymw{dayPoint};
	year_month_weekday_last ymwl = ymw.year() / ymw.month() / ymw.weekday()[last];

	return m_masks.DaysOfWeek[(unsigned)ymw.weekday()] ||
		(m_masks.DaysOfWeekLast[(unsigned)ymwl.weekday()] && sys_days{ymwl} == dayPoint) ||
		m_masks.DaysOfWeekIndex[(unsigned)ymwl.weekday() + (7u * (ymw.index() - 1u))];
}


bool TimeCalculator::IsDayOfMonthAllowed(const year_month_day& ymd) const
{
	return m_masks.DaysOfMonth[(unsigned)ymd.day()] ||
		(m_masks.DaysOfMonth[SheduleMasks::LastDayOfMonthBit] && ymd == (ymd.year() / ymd.month() / last));
}


bool TimeCalculator::IsDayOfMonthAllowed(const sys_days& dayPoint) const
{
	return IsDayOfMonthAllowed(year_month_day{dayPoint});
}

}  // namespace Cron
