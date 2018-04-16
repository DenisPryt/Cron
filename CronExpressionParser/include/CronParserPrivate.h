#pragma once

#include "CronSheduleMasks.h"
#include "CronSpecialSymbols.h"

#include <string_view>
#include <array>
#include <vector>


namespace Cron
{

class ParserPrivate
{
public:
	static constexpr const size_t MaxBit = std::numeric_limits<uint32_t>::max();
	static constexpr const size_t LastBit = std::numeric_limits<uint32_t>::max() - 1;

	enum FieldsNumbers : uint8_t
	{
		FieldSeconds     = 0, FieldMinutes = 1, FieldHours      = 2, 
		FieldDaysOfMonth = 3, FieldMonths  = 4, FieldDaysOfWeek = 5, 
		FieldsCount      = 6
	};


	void SetBits(uint32_t field, uint32_t valFrom, uint32_t valTo, const SpecialData& spec);

	std::pair<uint32_t, uint32_t> GetRangeValue(std::string_view& subField);

	SheduleMasks& Result() noexcept;
	const SheduleMasks& Result() const noexcept;


	static std::array<std::string, FieldsCount> SeparateFields(const std::string_view& shedule);
	static void SeparateSubFields(std::vector<std::string_view>& res, const std::string_view& field);

	static void ReplaceDaysOfWeekNames(std::string& field);
	static void ReplaceMonthsNames(std::string& field);

	static uint32_t ReadNumber(std::string_view& str);
	static SpecialSymbol ReadSpecialSymbol(std::string_view& str);
	static SpecialData ReadSpecialData(std::string_view& str);

private:
	SheduleMasks m_masks;
};

}  // namespace Cron
