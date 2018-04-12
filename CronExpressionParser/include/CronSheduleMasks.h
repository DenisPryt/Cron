#pragma once

#include <bitset>

namespace Cron
{
	struct SheduleMasks
	{
		constexpr static const size_t LastDayOfMonthBit = 32;

		inline bool IsEmpty() const noexcept
		{
			return Seconds.none() || Minutes.none() || Hours.none() || Months.none() || 
				(DaysOfWeek.none() && DaysOfWeekLast.none() && DaysOfWeekIndex.none() && DaysOfMonth.none());
		}

		std::bitset<60> Seconds{0};
		std::bitset<60> Minutes{0};
		std::bitset<24> Hours{0};

		std::bitset<13> Months{0}; // 1-12
		std::bitset<8> DaysOfWeek{0}; // 1-7
		std::bitset<8> DaysOfWeekLast{0}; // 1-7
		std::bitset<8 * 5> DaysOfWeekIndex{0}; // 1-7 x5
		std::bitset<33> DaysOfMonth{0}; // 1-31 + L
	};
}