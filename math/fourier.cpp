#include "fourier.hpp"

#include <cassert>
#include <cmath>

namespace pg
{

/**
 * @brief dftCT Discrete Fourier transform using Cooley-Tukey algorithm. Not
 *     exposed to the outside.
 * @param[out] spectrum The spectrum of 0th, mth, 2mth, ... elements of signal.
 * @param signal
 * @param length The length of the signal. Must be a power of 2.
 * @param modulo
 */
void dftCT(complex* const spectrum,
		   real const* const signal,
		   std::size_t length, std::size_t modulo);
} // namespace pg

// Implementations

void pg::dftCT(complex* const spectrum,
			   real const* const signal,
			   std::size_t length, std::size_t modulo)
{
	// Cooley-Tukey algorithm. See Wikipedia for details
	if (length == 1) // Terminal case
	{
		spectrum[0] = signal[0];
	}
	else
	{
		// Base: exp(-2 pi i / n)
		complex w(std::cos(2 * M_PI / length),
				  -std::sin(2 * M_PI / length));
		std::size_t mid = length / 2;
		dftCT(spectrum, signal, mid, 2 * modulo);
		dftCT(spectrum + mid, signal + modulo, mid, 2 * modulo);
		// Combine the DFTs
		for (std::size_t i = 0; i < mid; ++i)
		{
			complex temp = spectrum[i];
			complex transformed = std::pow(w, i) * spectrum[i + mid];
			spectrum[i] = temp + transformed;
			spectrum[i + mid] = temp - transformed;
		}
	}
}

void pg::dft(complex* const spectrum,
			 real const* const signal, std::size_t length)
{
	dftCT(spectrum, signal, length, 1);
}

void pg::dstft(complex* const* const spectrogram,
			   real const* const signal, std::size_t length,
			   real const* const window, std::size_t r,
			   std::size_t p, std::size_t q,
			   real* const integrand)
{
	assert(start <= end);
	/*
	 * Let length be n, start be sp, end be sq, and windowRadius be r. Let the
	 * signal be represented as A[0..n[, the spectrogram be S[0..n[[0..2r[,
	 * and the window be W[0..2r[
	 */

	/*
	 * If we assume that A is padded with zeroes, the main loop can be expressed
	 * as (in pseudocode)
	 *
	 * for j <- [p..q[
	 *     S[j] = DFT CORRELATE(W[0..2r[, A[j - r, j + r[)
	 * end for
	 *
	 * where the CORRELATE subroutine is a simple loop dot multiplying
	 * corresponding elements of W and A.
	 * In the real routine, it is much more efficient to emulate padding rather
	 * than actually allocating memory before and after A.
	 *
	 * For any j <- [p..q[, the range [j - r, j + r[ touches at least one
	 * negative index if j - r < 0 with at most r - j paddings. Hence in total
	 * r - p paddings are needed. Since paddings are erased as A fills into
	 * succeeding elements, the paddings can be initialised only once.
	 */
	if (r > p)
		for (std::size_t i = 0; i < r - p; ++i)
			integrand[i] = 0.0;

	// TODO: Finish this

	// Pre padding
	for (std::size_t i = 0; i < windowRadius; ++i)
		integrand[i] = 0.0;


	// Window
	if (end - start < windowRadius)
	{
	}
	else if (end - start < 2 * windowRadius)
	{
		for (index = 0; index < length; ++index)
		{
			// Padding
			if (windowRadius > index)
				for (std::size_t i = 0; i < windowRadius - index; ++i)
					integrand[i] = 0.0;


			for (std::size_t i = windowRadius - index; i < windowRadius * 2; ++i)
			{
				integrand[i] = signal[i + index - windowRadius] * window[i];
			}
		}
	}


}
