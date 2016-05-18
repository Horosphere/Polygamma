#ifndef POLYGAMMA_HPP
#define POLYGAMMA_HPP

#include <complex>

namespace pg
{
/** Currently using 64 bits floating point.
 * @brief real The type used by PolyGamma to represent real numbers.
 */
typedef double real;
typedef std::complex<real> complex;

constexpr std::size_t const AUDIO_CHUNK_SIZE = 8192;

}


#endif // POLYGAMMA_HPP
