#include "CronExpression.h"
#include "CronParserPrivate.h"
#include "CronTimeCalculatorPrivate.h"

namespace Cron
{

//static
Expression Expression::ParseCronExpression(const std::string_view& shedule)
{
	ParserPrivate p;

	auto fields = p.SeparateFields(shedule);

	p.ReplaceDaysOfWeekNames(fields.back());
	p.ReplaceMonthsNames(fields[fields.size() - 2]);

	std::vector<std::string_view> subFields;

	for (uint32_t fieldNum = 0; fieldNum < fields.size(); ++fieldNum)
	{
		auto& field = fields[fieldNum];

		p.SeparateSubFields(subFields, field);

		for (auto& subField : subFields)
		{
			std::string_view subFieldView = subField;

			uint32_t valFrom, valTo;
			std::tie(valFrom, valTo) = p.GetRangeValue(subFieldView);

			p.SetBits(fieldNum, valFrom, valTo, p.ReadSpecialData(subFieldView));
		}
	}

	Expression res;
	res.m_masks = std::move(p.Result());

	return res;
}


std::chrono::system_clock::time_point Expression::NextTime(const std::chrono::system_clock::time_point& from)
{
	return TimeCalculator{m_masks}.Next(from);
}


std::chrono::system_clock::time_point Expression::PrevTime(const std::chrono::system_clock::time_point& from)
{
    return TimeCalculator{m_masks}.Prev(from);
}
    

bool Expression::IsEmpty() const noexcept
{
	return m_masks.IsEmpty();
}

}
