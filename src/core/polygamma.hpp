#ifndef _POLYGAMMA_CORE_POLYGAMMA_HPP__
#define _POLYGAMMA_CORE_POLYGAMMA_HPP__

#include <complex>
#include <string>

namespace pg
{
/** Currently using 64 bits floating point.
 * @brief real The type used by PolyGamma to represent real numbers.
 */
typedef double real;
typedef std::complex<real> complex;

std::string about();

} // namespace pg

// Implementations

inline std::string pg::about()
{
	return "Polygamma 0.0.1\n"
	       "Author: Horosphere\n";
}

#endif // !_POLYGAMMA_CORE_POLYGAMMA_HPP__

