#include "CronSpecialSymbols.h"

#include <cctype>


namespace Cron
{

bool SpecialSymbols::IsLast(char ch) noexcept
{
	return ch == 'L' || ch == 'l';
}


bool SpecialSymbols::IsWeekday(char ch) noexcept
{
	return ch == 'W' || ch == 'w';
}


bool SpecialSymbols::IsHash(char ch) noexcept
{
	return ch == '#';
}


bool SpecialSymbols::IsSlash(char ch) noexcept
{
	return ch == '/';
}


bool SpecialSymbols::IsDigitOrLast(char ch) noexcept
{
	return std::isdigit(static_cast<unsigned char>(ch)) || IsLast(ch);
}


void SpecialSymbols::ThrowIfInvalid(const SpecialData& spec)
{
	switch (spec.symbol)
	{
	case Hash:
		if (spec.value < 1 || spec.value > 5)
			throw std::runtime_error("Cron parser error: number after # must be 1-5");

		break;

	case Slash:
		if (spec.value == 0)
			throw std::runtime_error("Cron parser error: number after / must be greater then 0");

		break;
	}
}

}  // namespace Cron