#pragma once

#include "CronSheduleMasks.h"

#include <string_view>
#include <chrono>


namespace Cron
{

class Expression
{
public:
	Expression() = default;
	Expression(Expression&&) = default;
	Expression(const Expression&) = default;
	Expression& operator=(Expression&&) = default;
	Expression& operator=(const Expression&) = default;

	static Expression ParseCronExpression(const std::string_view& cronExpr);

	std::chrono::system_clock::time_point NextTime(const std::chrono::system_clock::time_point& from);

	bool IsEmpty() const noexcept;

private:
	SheduleMasks m_masks;
};

}

