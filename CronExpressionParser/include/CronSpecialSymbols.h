#pragma once

#include <stdint.h>

namespace Cron
{

enum SpecialSymbol : uint8_t
{
	Slash = 0, Hash, L, W, LW, None
};


struct SpecialData
{
	SpecialSymbol symbol = None;
	uint32_t value = 0;
};


class SpecialSymbols
{
	SpecialSymbols() = delete;
	~SpecialSymbols() = delete;
public:

	static bool IsLast(char ch) noexcept;
	static bool IsWeekday(char ch) noexcept;
	static bool IsHash(char ch) noexcept;
	static bool IsSlash(char ch) noexcept;
	static bool IsDigitOrLast(char ch) noexcept;

	static void ThrowIfInvalid(const SpecialData& spec);
};

}  // namespace Cron


