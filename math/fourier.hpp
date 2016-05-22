#ifndef FOURIER_HPP
#define FOURIER_HPP

#include "../core/polygamma.hpp"

namespace pg
{


/**
 * Implemented using fft algorithms. This function is not responsible for any
 * allocation and padding.
 * @brief dft Discrete Fourier transform
 * @param[out] spectrum The spectrum of the signal. Should be allocated to have
 *     length of at least length.
 * @param[in] signal The signal.
 * @param[in] length The length of the signal. Must be >= 1 and a power of 2
 */
void dft(complex* const spectrum, real const* const signal, std::size_t length);

/**
 * This function is not responsible for any allocation.
 * @brief dstft Discrete short-time Fourier transform
 * @param[out] spectrogram A two dimensional row major matrix that will be
 *     filled with the spectrogram of the signal. The width of this matrix
 *     should be equal to length and the height should be windowRadius * 2.
 * @param[in] signal The signal. Will be correlated(not convolved!) against
 *     window to produce the spectrogram.
 * @param[in] length The length of the signal. Must be >= 1
 * @param[in] window The window function that will be used to calculate the
 *     transform. window[0] to window[2 * r - 1] should be allocated.
 * @param[in] windowRadius The radius of the window. Must be >= 1 and a power of
 *     2. The peak of the window should be window[r - 1] and window[r]. The
 *     latter is considered to be the zero.
 * @param[in] start The interval [start,end[ of the spectrogram will be
 *     computed, with everything else left untouched.
 *     start and end should satisfy 0 <= start <= end < length
 * @param[in] end See \ref start.
 * @param integrand Temporary memory allocated with size 2 * windowRadius
 */
void dstft(complex* const* const spectrogram,
		   real const* const signal, std::size_t length,
		   real const* const window, std::size_t windowRadius,
		   std::size_t start, std::size_t end,
		   real* const integrand);

} // namespace pg

#endif // FOURIER_HPP
