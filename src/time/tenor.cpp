#include "tenor.h"

#include <cctype>
#include <cstddef>
#include <ostream>
#include <string>

#include "helpers/utils.h"

namespace oa {
namespace time {

////////////////////////////////////////////////////////////////////////////////
// Tenor::GroupLess                                                           //
////////////////////////////////////////////////////////////////////////////////

bool
Tenor::GroupLess::operator()(const Tenor& a, const Tenor& b) const noexcept
{
	// convert enum to ranking
	// TODO: can have this as a free function later
	auto rank = [](Tenors u) noexcept
	{
		switch (u) {
		case Tenors::kDays:
			return 0u;
		case Tenors::kWeeks:
			return 1u;
		case Tenors::kMonths:
			return 2u;
		case Tenors::kYears:
			return 3u;
		// bogus ranking value to ensure bad values are ordered last
		default:
			return 100u;
		}
	};
	// get a and b rank
	auto a_rank = rank(a.unit());
	auto b_rank = rank(b.unit());
	// if rank is the same, compare counts, otherwise compare ranks
	return (a_rank == b_rank) ? (a.count() < b.count()) : (a_rank < b_rank);
}

////////////////////////////////////////////////////////////////////////////////
// Tenor                                                                      //
////////////////////////////////////////////////////////////////////////////////

// TODO: clean this function up later
Tenor::Tenor(const std::string& input_string)
{
	if (oa::utils::CheckTenorStr(input_string))
	{
		m_number_ = std::stoi(input_string.substr(0, input_string.size() - 1));
		m_time_unit_ = oa::utils::ValueToEnum<time::Tenors>(std::toupper(input_string.back()));
	}
	else
		throw "Invalid tenor type please check your input of " + input_string;
}

Tenor::Tenor(int count, Tenors unit) noexcept
  : m_number_{count}, m_time_unit_{unit}
{}

Tenor Tenor::FlipSign() const
{
	// note: old implementation used the std::string ctor which is inefficient
	return {-count(), unit()};
}

std::pair<int, Tenors>Tenor::GetValues() const
{
	return {m_number_, m_time_unit_};
}

int
Tenor::count() const noexcept
{
	return m_number_;
}

Tenors
Tenor::unit() const noexcept
{
	return m_time_unit_;
}

Tenor
Tenor::operator-() const noexcept
{
	return {-m_number_, m_time_unit_};
}

bool
Tenor::operator==(const Tenor& other) const noexcept
{
	return (count() == other.count()) && (unit() == other.unit());
}

std::size_t
Tenor::hash() const noexcept
{
	// FNV prime
	constexpr auto prime = []() -> std::size_t
	{
		if constexpr (sizeof(std::size_t) == 4u)  // 32 bit
			return 0x1000193;
		else
			return 0x100000001b3;                 // 64 bit
	}();
	// initial result (offset)
	auto result = []() -> std::size_t
	{
		if constexpr (sizeof(std::size_t) == 4u)  // 32 bit
			return 0x811c9dc5;
		else
			return 0xcbf29ce484222325;            // 64 bit
	}();
	// hash tenor count byte-by-byte
	{
		auto num = count();
		// for each byte do multiply + XOR
		for (auto i = 0u; i < sizeof(decltype(num)); i++) {
			result *= prime;
			result ^= (0xFF & num);
			num >>= 8;
		}
	}
	// tenor unit fits in one byte so only hash low byte
	result *= prime;
	result ^= (0xFF & static_cast<int>(unit()));
	// done
	return result;
}

Tenor
Tenor::operator+(int shift) const noexcept
{
	return {count() + shift, unit()};
}

Tenor
Tenor::operator-(int shift) const noexcept
{
	return {count() - shift, unit()};
}

////////////////////////////////////////////////////////////////////////////////
// Tenor non-member operators                                                 //
////////////////////////////////////////////////////////////////////////////////

Tenor operator+(int shift, const Tenor& tenor) noexcept
{
	return tenor + shift;  // calls Tenor::operator+
}

std::ostream& operator<<(std::ostream& out, const Tenor& tenor)
{
	// convert enum value to lowercase (ASCII offset of 'a' - 'A' is 32)
	auto unit = static_cast<char>(static_cast<int>(tenor.unit()) + 32);
	// write
	return out << tenor.count() << unit;
}

}  // namespace time
}  // namespace oa
