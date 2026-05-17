#ifndef OA_TIME_TENOR_H_
#define OA_TIME_TENOR_H_

#include <cstddef>
#include <iosfwd>
#include <string>

#include "oa/dllexport.h"
#include "time_enums.h"

namespace oa::time {

class OA_TIME_API Tenor {
public:
	/**
	 * `Tenor` comparison functor that groups relation by time unit.
	 *
	 * This provides a strict weak ordering of `Tenor` objects based on their
	 * time unit and then provides a strict ordering within each time unit
	 * category based on the unit count. For example, any `Tenor` that is in
	 * days is automatically ordered before a `Tenor` in weeks, even if the
	 * number of days exceeds 7, e.g. `12d` is ordered before `1w`. Another
	 * example would be `24m` being ordered before `1y`.
	 *
	 * Although this property makes this *Compare* functor inappropriate for
	 * imposing a total ordering in an arithmetic sense, grouping tenors by
	 * unit provides an ordering familiar to traders, and allows use with
	 * `std::map` so a `Tenor` can be used as a key.
	 */
	struct OA_TIME_API GroupLess {
		/**
		 * Indicate if one `Tenor` is ordered before the other.
		 *
		 * Ordering is strictly grouped based on the length of the time unit
		 * while intra-group ordering is strict based on count.
		 *
		 * @param a First tenor
		 * @param b Second tenor
		 */
		bool operator()(const Tenor& a, const Tenor& b) const noexcept;
	};

	/**
	 * Ctor.
	 *
	 * Construct from a valid tenor string like `"3w"`. The case of the given
	 * time unit can be lower or upper case, i.e. `"3W"` is also allowed.
	 *
	 * @todo Should use `string_view` to avoid needing to create a string.
	 *
	 * @param str Tenor string, e.g. `"2y"` or `"2Y"`
	 */
	Tenor(const std::string& str);

	/**
	 * Ctor.
	 *
	 * Construct the tenor from a time unit and the number of time units.
	 *
	 * @param count Number of time units
	 * @param unit Tenor time unit enum value
	 */
	Tenor(int count, Tenors unit) noexcept;

	// TODO: deprecate in favor of unary negation
	/// <summary>
	/// flips the sign of the tenor
	/// </summary>
	/// <returns>New Tenor object withteh sign flipped</returns>
	Tenor FlipSign() const;

	// TODO: deprecate in favor of just using count() and unit()
	/// <summary>
	/// returns a pair that is the integer and the unit of time a char
	/// </summary>
	/// <returns>a pair where the first length and the second argument is lenght as a Enum</returns>
	std::pair<int, Tenors> GetValues() const;

	/**
	 * Return the number of units represented by the tenor.
	 */
	int count() const noexcept;

	/**
	 * Return the time unit value.
	 */
	Tenors unit() const noexcept;

	/**
	 * Flip the sign of the tenor.
	 *
	 * This produces a new `Tenor` object of same unit with negated count.
	 */
	Tenor operator-() const noexcept;

	/**
	 * Compare against another tenor for equality.
	 *
	 * Tenor equality is done strictly in terms of the count + unit together.
	 * Therefore, a tenor of 7d is *not* considered equal to 1w.
	 *
	 * @param other Tenor to compare against
	 */
	bool operator==(const Tenor& other) const noexcept;

	/**
	 * Return a hash value for the `Tenor`.
	 *
	 * The hash value is computed for 32- and 64-bit platforms using the
	 * FNV-1 hash algorithm. In theory, however, if we restricted the value
	 * of count to fit within 3 bytes, then we could use this implementation:
	 *
	 * @code{.cc}
	 * (std::size_t{count()} << 8) | (0xFF & static_cast<int>(unit()))
	 * @endcode
	 *
	 * This takes advantage of the fact that the values of the `Tenors` enum
	 * all nicely fit within a single byte so we only need the low byte.
	 */
	std::size_t hash() const noexcept;

	/**
	 * Add the given count to create a new `Tenor` object.
	 *
	 * @param shift Unit quantity to add
	 */
	Tenor operator+(int shift) const noexcept;

	/**
	 * Subtract the given count to create a new `Tenor` object.
	 *
	 * @note There is no accompanying operator for `int` - `Tenor`.
	 *
	 * @param shift Unit quantity to subtract
	 */
	Tenor operator-(int shift) const noexcept;

private:
	// TODO: rename members so they are semantically consistent with getters
	int m_number_;
	Tenors m_time_unit_;
};

/**
 * Add the given count to create a new `Tenor` object.
 *
 * @param shift Unit quantity to add
 * @param tenor Tenor to adjust
 */
OA_TIME_API
Tenor operator+(int shift, const Tenor& tenor) noexcept;

/**
 * Write the `Tenor` to the output stream.
 *
 * This will write the natural language form of a tenor, e.g. `"3m"`.
 *
 * @param out Output stream
 * @param tenor Tenor to write
 */
OA_TIME_API
std::ostream& operator<<(std::ostream& out, const Tenor& tenor);

}  // namespace oa::time

namespace std {

/**
 * `std::hash` specialization for the `Tenor`.
 */
template <>
struct hash<oa::time::Tenor> {
	/**
	 * Return the value of `Tenor::hash()` on the given `Tenor`.
	 */
	auto operator()(const oa::time::Tenor& tenor) const noexcept
	{
		return tenor.hash();
	}
};

}  // namespace std

#endif  // OA_TIME_TENOR_H_
