#ifndef INTERVAL_HPP
#define INTERVAL_HPP

#include <cassert>
#include <algorithm>

namespace pg
{

/**
 * @brief The Interval struct represents one axis indexed by two real
 * (double floating point) numbers. The utility methods \ref scale and \ref
 * clamp are used in conjunction.
 *
 * The template parameter typename R is for real.
 */
template<typename R>
struct Interval
{
	/**
	 * The default construct shall never be called unless we are allocating new
	 * memory.
	 */
	Interval()
	{
	}
	Interval(R begin, R end): begin(begin), end(end)
	{
		assert(begin <= end); // Sanity check
	}
	R begin, end;

	Interval<R>& operator=(Interval<R> const&) = default;
	Interval<R>& operator+=(R v)
	{
		begin += v; end += v;
		return *this;
	}
	Interval<R>& operator-=(R v)
	{
		begin -= v; end -= v;
		return *this;
	}

};

template<typename R> Interval<R>
translate(Interval<R> const&, Interval<R> const& limits, R);

template<typename R, typename Fac> Interval<R>
/**
 * @warning If typename R is an unsigned integer, centre must be within the
 *      interval.
 * @brief scale Scales the given interval with respect to centre and ratio fac.
 * @param fac The ratio of scaling.
 * @param centre The centre of scaling. If this point is contained in the
 * interval, it will remain in the interval after scaling with arbitrary
 * fac.
 * @returns The scaled interval.
 */
scale(Interval<R> const&, Fac fac, R centre);

// If typename R is an unsigned integer,
// Centre must be within the interval which must be within the limits
template<typename R, typename Fac> Interval<R>
scale(Interval<R> const&, Interval<R> const& limits, Fac fac, R centre);

template<typename R> R length(Interval<R> const&);

} // namespace pg



// Implementations

template<typename R> inline pg::Interval<R>
pg::translate(Interval<R> const& interval, Interval<R> const& bound, R val)
{
	if (val > bound.end - interval.end)
		return Interval<R>(bound.end - length(interval), bound.end);
	if (val < bound.begin - interval.begin)
		return Interval<R>(bound.begin, bound.begin + length(interval));

	return Interval<R>(interval.begin + val, interval.end + val);
}

template<typename R, typename Fac> inline pg::Interval<R>
pg::scale(Interval<R> const& interval, Fac fac, R centre)
{
	return Interval<R>((R)((interval.begin - centre) * fac) + centre,
					(R)((interval.end - centre) * fac) + centre);
}

template<typename R, typename Fac> inline pg::Interval<R>
pg::scale(Interval<R> const& interval, Interval<R> const& limits,
		  Fac fac, R centre)
{
	// The std::min here is introduced to prevent overflow
	// Overflow on left
	if (centre - limits.begin < (centre - interval.begin) * fac)
	{
		return Interval<R>(limits.begin, limits.begin
						   + std::min((R)(length(interval) * fac), length(limits)));
	}
	// Overflow on right
	if (limits.end - centre < (interval.end - centre) * fac)
	{
		return Interval<R>(limits.end
						  - std::min((R)(length(interval) * fac), length(limits)),
						  limits.end);
	}
	// Nothing happens
	return scale(interval, fac, centre);

}

template<typename R> inline R
pg::length(Interval<R> const& interval)
{
	return interval.end - interval.begin;
}

#endif // INTERVAL_HPP
