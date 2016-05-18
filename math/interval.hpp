#ifndef INTERVAL_HPP
#define INTERVAL_HPP

#include <cassert>

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
/**
 * @brief scale Scales the given interval with respect to centre and ratio fac.
 * @param fac The ratio of scaling.
 * @param centre The centre of scaling. If this point is contained in the
 * interval, it will remain in the interval after scaling with arbitrary
 * fac.
 * @returns The scaled interval.
 */
scale(Interval<R> const&, R fac, R centre);

template<typename R> Interval<R>
scale(Interval<R> const&, Interval<R> const& limits, R fac, R centre);

template<typename R> R length(Interval<R> const&);

} // namespace pg

// Implementations

template<typename R> inline pg::Interval<R>
pg::scale(Interval<R> const& interval, R fac, R centre)
{
	return Interval<R>((interval.begin - centre) * fac + centre,
					(interval.end - centre) * fac + centre);
}

template<typename R> inline pg::Interval<R>
pg::scale(Interval<R> const& interval, Interval<R> const& limits,
		  R fac, R centre)
{
	Interval<R> scaled = scale(interval, fac, centre);
	if (scaled.begin < limits.begin)
	{
		scaled.end += limits.begin - scaled.begin;
		scaled.begin = limits.begin;
		if (scaled.end <= limits.end)
			return scaled;
		else return limits;
	}
	else if (scaled.end > limits.end)
	{
		scaled.begin += limits.end - scaled.end;
		scaled.end = limits.end;
		if (scaled.begin >= limits.begin)
			return scaled;
		else return limits;
	}
	else return scaled;
}

template<typename R> inline R
pg::length(Interval<R> const& interval)
{
	return interval.end - interval.begin;
}

#endif // INTERVAL_HPP
