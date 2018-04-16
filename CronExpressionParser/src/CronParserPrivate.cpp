#include "CronParserPrivate.h"

#include <cctype>

using std::string_view;

namespace
{

template<size_t N>
void SetBits(std::bitset<N>& bits, uint32_t field, uint32_t bitFrom, uint32_t bitTo, uint32_t step)
{
	if (bitFrom > bitTo)
		throw std::runtime_error("Cron parser error: beginning of the sequence is greater than the end");

	if (bitTo == Cron::ParserPrivate::MaxBit)
	{
		bitTo = (uint32_t)bits.size() - 1;
	}

	if (bitTo >= bits.size())
		throw std::runtime_error("Cron parser error: end of the sequence is greater than max possible value");

	for (; bitFrom <= bitTo; bitFrom += step)
	{
		bits.set(bitFrom);
	}
}


template<class Names_t>
void ReplaceToPosition(std::string& field, const Names_t& names)
{
	for (size_t i = 0; i < names.size(); ++i)
	{
		if (std::empty(names[i]))
			continue;

		auto res = std::search(std::begin(field), std::end(field), std::begin(names[i]), std::end(names[i]),
							   [](unsigned l, unsigned r) { return std::toupper(l) == std::toupper(r); });

		if (res == field.end())
			continue;

		field.replace(res, res + names[i].size(), std::to_string(i));
	}
}


template<class StringOut_t, class StringIn_t>
void SplitString(std::vector<StringOut_t>& res, const StringIn_t& strIn, const string_view& token)
{
    using It_t = typename StringIn_t::iterator;
	using CategoryIn_t = typename std::iterator_traits<It_t>::iterator_category;
	static_assert(std::is_same_v<CategoryIn_t, std::random_access_iterator_tag>, "string iterator must be random access");

	res.clear();

	auto searchFrom = std::cbegin(strIn);
	auto strEnd = std::cend(strIn);

	while (true)
	{
		auto searchRes = std::search(searchFrom, strEnd, std::cbegin(token), std::cend(token));

		if (searchRes == strEnd)
			break;

		res.emplace_back(&*searchFrom, std::distance(searchFrom, searchRes));
		searchFrom = std::next(searchRes);
	}

	res.emplace_back(&*searchFrom, std::distance(searchFrom, strEnd));
}

}  // namespace


namespace Cron
{

void ParserPrivate::SetBits(uint32_t field, uint32_t valFrom, uint32_t valTo, const SpecialData& spec)
{
	if (valFrom > valTo)
		throw std::runtime_error("Cron parser error: beginning of the sequence is greater than the end");

	uint32_t step = spec.symbol == Slash ? spec.value : 1u;

	switch (field)
	{
	case FieldSeconds:
		::SetBits(m_masks.Seconds, field, valFrom, valTo, step);
		break;

	case FieldMinutes:
		::SetBits(m_masks.Minutes, field, valFrom, valTo, step);
		break;

	case FieldHours:
		::SetBits(m_masks.Hours, field, valFrom, valTo, step);
		break;

	case FieldDaysOfMonth:
		if (valFrom == LastBit)
			valFrom = SheduleMasks::LastDayOfMonthBit;

		if (valTo == LastBit)
			valTo = SheduleMasks::LastDayOfMonthBit;

		::SetBits(m_masks.DaysOfMonth, field, valFrom, valTo, step);
		break;

	case FieldMonths:
		if (valFrom > 12)
			throw std::runtime_error("Cron parser: invalid month number " + std::to_string(valFrom) + ". Valid values [1-12]");

		::SetBits(m_masks.Months, field, valFrom, valTo, step);
		break;

	case FieldDaysOfWeek:
		if (valFrom == 0)
			::SetBits(m_masks.DaysOfWeek, field, 7, 7, 1);

		if (spec.symbol == L)
		{
			::SetBits(m_masks.DaysOfWeekLast, field, valFrom, valTo, step);
		}
		else if (spec.symbol == Hash)
		{
			uint32_t offset = 7u * (spec.value - 1u);
			::SetBits(m_masks.DaysOfWeekIndex, field, valFrom + offset, valTo + offset, step);
		}
		else
		{
			::SetBits(m_masks.DaysOfWeek, field, valFrom, valTo, step);
		}
		break;

	default:
		throw std::out_of_range{"Cron parse error: SetBits field " + std::to_string(field) + " is invalid"};
	}
}


std::pair<uint32_t, uint32_t> ParserPrivate::GetRangeValue(string_view& subField)
{
	auto dash = std::find(subField.cbegin(), subField.cend(), '-');
	if (dash != subField.cend())
	{
		if (std::prev(dash) == subField.cend() || !SpecialSymbols::IsDigitOrLast(*std::prev(dash)))
			throw std::runtime_error("Cron parse error: not a number or L symbol before dash");

		if (std::next(dash) == subField.cend() || !SpecialSymbols::IsDigitOrLast(*std::next(dash)))
			throw std::runtime_error("Cron parse error: not a number or L symbol after dash");

		size_t dashPos = std::distance(subField.cbegin(), dash);
		string_view from = subField.substr(0, dashPos);
		string_view to = subField.substr(dashPos + 1);

		std::pair<uint32_t, uint32_t> res = {ReadNumber(from), ReadNumber(to)};

		subField = to;

		return res;
	}

	if (subField.empty())
	{
		throw std::runtime_error("Cron parser error: empty subfield");
	}

	if (subField.front() == '/')
	{
		return {0, (uint32_t)MaxBit};
	}

	if (subField.front() == '*' || subField.front() == '?')
	{
		subField.remove_prefix(1);
		return {0, (uint32_t)MaxBit};
	}

	if (!SpecialSymbols::IsDigitOrLast(subField.front()))
		throw std::runtime_error("Cron parse error: invalid symbol");


	uint32_t val = ReadNumber(subField);

	if (!subField.empty() && subField.front() == '/')
		return {val, (uint32_t)MaxBit};


	return {val, val};
}


//static
auto ParserPrivate::SeparateFields(const string_view& shedule) -> std::array<std::string, FieldsCount>
{
	std::vector<string_view> fields;
	fields.reserve(FieldsCount);
	SplitString(fields, shedule, " ");

	if (fields.size() != FieldsCount - 1 && 
		fields.size() != FieldsCount)
	{
		throw std::runtime_error("Cron parse error: shedule must consist of " + std::to_string(FieldsCount - 1) +
								 " or " + std::to_string(FieldsCount) + " fields");
	}


	std::array<std::string, FieldsCount> res;
	auto resIt = res.begin();
	
	if (fields.size() == FieldsCount - 1)
	{
		*resIt++ = "0";
	}

	for (auto&& field : fields)
	{
		*resIt++ = field;
	}

	return res;
}


//static
void ParserPrivate::SeparateSubFields(std::vector<string_view>& res, const string_view& field)
{
	SplitString(res, field, ",");
}


//static
void ParserPrivate::ReplaceDaysOfWeekNames(std::string& field)
{
	static const std::array<std::string, 7> Names = {
		"SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
	};

	::ReplaceToPosition(field, Names);
}


//static
void ParserPrivate::ReplaceMonthsNames(std::string& field)
{
	static const std::array<std::string, 14> Names = {
		"", "FOO", "JAN", "FEB", "MAR", "APR", "MAY", "JUN", "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
	};

	::ReplaceToPosition(field, Names);
}


//static
uint32_t ParserPrivate::ReadNumber(string_view& str)
{
	if (str.empty())
		throw std::runtime_error("Cron parse error: not a number or L");


	if (SpecialSymbols::IsLast(str.front()))
	{
		str.remove_prefix(1);
		return (uint32_t)LastBit;
	}

	char* nextSymbol = nullptr;
	uint32_t num = (uint32_t)std::strtoul(str.data(), &nextSymbol, 10);

	if (nextSymbol == str)
		throw std::runtime_error("Cron parse error: not a number or L");

	str.remove_prefix(nextSymbol - str.data());

	return num;
}


//static
SpecialSymbol ParserPrivate::ReadSpecialSymbol(string_view& str)
{
	if (SpecialSymbols::IsLast(str.front()))
	{
		str.remove_prefix(1);

		if (!str.empty() && SpecialSymbols::IsWeekday(str.front()))
		{
			str.remove_prefix(1);
			return LW;
		}

		return L;
	}
	else if (SpecialSymbols::IsWeekday(str.front()))
	{
		str.remove_prefix(1);
		return W;
	}
	else if (SpecialSymbols::IsHash(str.front()))
	{
		str.remove_prefix(1);
		return Hash;
	}
	else if (SpecialSymbols::IsSlash(str.front()))
	{
		str.remove_prefix(1);
		return Slash;
	}

	return None;
}


//static
SpecialData ParserPrivate::ReadSpecialData(string_view& str)
{
	if (str.empty())
		return {};

	SpecialData res;
	res.symbol = ReadSpecialSymbol(str);

	if (res.symbol == Hash || res.symbol == Slash)
		res.value = ReadNumber(str);

	SpecialSymbols::ThrowIfInvalid(res);

	return res;
}


SheduleMasks& ParserPrivate::Result() noexcept
{
	return m_masks;
}


const SheduleMasks& ParserPrivate::Result() const noexcept
{
	return m_masks;
}

}  // namespace Cron
