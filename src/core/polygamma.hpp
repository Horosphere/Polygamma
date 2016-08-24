/*
 * Convention:
 * Types with trivial constructor, destructor, and copy/move constructor/
 * assignment start with lower case letter. All other types start with upper
 * case letter.
 * Use camelCase instead of underscore.
 */
#ifndef _POLYGAMMA_CORE_POLYGAMMA_HPP__
#define _POLYGAMMA_CORE_POLYGAMMA_HPP__

#include <complex>
#include <string>

#ifndef NDEBUG
#include <boost/timer/timer.hpp>
#define DEBUG_TIMER_BEGIN boost::timer::cpu_timer debugTimer
#define DEBUG_TIMER_END(text) std::cout << text << debugTimer.format()
#else
#define DEBUG_TIMER_BEGIN
#define DEBUG_TIMER_END(text)
#endif

namespace pg
{
/** Currently using 64 bits floating point.
 * @brief real The type used by PolyGamma to represent real numbers.
 */
typedef double real;
typedef std::complex<real> complex;

typedef uint64_t ChannelLayout;

/**
 * @brief Returns the version information.
 */
std::string about();


// Implementations

inline std::string about()
{
	return "Polygamma 0.0.1\n"
	       "Author: Horosphere\n";
}


} // namespace pg

#endif // !_POLYGAMMA_CORE_POLYGAMMA_HPP__

