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

void pg::windowRect(real* const window, std::size_t length)
{
	for (std::size_t j = 0; j < length; ++j)
		window[j] = 1.0;
}
void pg::windowGaussian(real* const window, std::size_t length, real sigma)
{
	real fac = 2.0 / (sigma * length);
	for (std::size_t j = 0; j < length; ++j)
	{
		real temp = (j - length * 0.5) * fac;
		window[j] = std::exp(-0.5 * temp * temp);
	}
}

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

// The arguments here are renamed
void pg::dstft(complex* const* const spectrogram,
			   real const* const signal, std::size_t n,
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
	 * r - p paddings are needed. Since paddings are erased as A fill into
	 * succeeding elements, the paddings can be initialised only once.
	 *
	 * The range [j - r, j + r[ touches at least one out of bound index (n) if
	 * j + r > n. That is, j > n - r. In some cases a j exists so that
	 * [j - r, j + r[ touches both ends. Solving the inequalities j - r < 0 and
	 * j > n - r gives n < 2r.
	 *
	 * Critical points:
	 * (n >= 2r) r, n - r (n < 2r) n - r, r
	 */
	if (n < 2 * r)
	{
		// Fills in the padding zeroes
		if (p < r)
			for (std::size_t k = 0; k < r - p; ++k)
				integrand[k] = 0.0;

		/*
		 * This loop corresponds to underflow but not overflow.
		 * Using p + r < n instead of p < n - r to avoid unsigned arithmetic
		 * overflow.
		 */
		if (p + r < n)
		{
			// n - r cannot be less than 0
			std::size_t loopMax = std::min(q, n - r);
			for (std::size_t j = p; j < loopMax; ++j)
			{
				for (std::size_t k = r - j; k < 2 * r; ++k)
				{
					integrand[k] = window[k] * signal[k + j - r];
				}
				dft(spectrogram[j], integrand, 2 * r);
			}
		}
		/*
		 * This loop corresponds to underflow and overflow
		 * Using q + r > n instead of q > n - r to avoid unsigned arithmetic
		 * overflow.
		 */
		if (p < r && q + r > n)
		{
			// This should be max(p, n - r) if we are using signed arithmetic.
			std::size_t loopMin = n <= r ? p : std::max(p, n - r);
			std::size_t loopMax = std::min(q, r);

			// Zeroes written to the beginning/end
			for (std::size_t k = 0; k < 2 * r; ++k)
				integrand[k] = 0;
			for (std::size_t j = loopMin; j < loopMax; ++j)
			{
				integrand[n + r - j] = 0;
				/*
				 * r - j is guarenteed to not underflow.
				 * Overflow occurs when k + j - r >= n. Rearranging gives
				 * k >= n + r - j, which establishes the upper limit
				 */
				for (std::size_t k = r - j; k < n + r - j; ++k)
				{
					integrand[k] = window[k] * signal[k + j - r];
				}
				dft(spectrogram[j], integrand, 2 * r);
			}
		}
		/*
		 * This loop corresponds to overflow but not underflow.
		 * If n < r, this condition can never be true, so n - r is guarenteed to
		 * not cause an arithmetic overflow
		 */
		if (q > r)
		{
			std::size_t loopMin = std::max(p, r);
			for (std::size_t k = n + r - loopMin; k < 2 * r; ++k)
				integrand[k] = 0.0;
			for (std::size_t j = loopMin; j < q; ++j)
			{
				integrand[n + r - j] = 0.0;
				for (std::size_t k = 0; k < n + r - j; ++k)
				{
					integrand[k] = window[k] * signal[k + j - r];
				}
				dft(spectrogram[j], integrand, 2 * r);
			}
		}
	}
	else
	{
		if (p < r)
		{
			// Fills in the padding zeroes
			for (std::size_t k = 0; k < r - p; ++k)
				integrand[k] = 0.0;

			std::size_t loopMax = std::min(r, q);
			for (std::size_t j = p; j < loopMax; ++j)
			{
				for (std::size_t k = r - j; k < 2 * r; ++k)
				{
					integrand[k] = window[k] * signal[k + j - r];
				}
				dft(spectrogram[j], integrand, 2 * r);
			}
		}
		if (p < n - r && q > r)
		{
			std::size_t loopMin = std::max(p, r);
			std::size_t loopMax = std::min(q, n - r);
			for (std::size_t j = loopMin; j < loopMax; ++j)
			{
				for (std::size_t k = 0; k < 2 * r; ++k)
				{
					integrand[k] = window[k] * signal[k + j - r];
				}
				dft(spectrogram[j], integrand, 2 * r);
			}
		}
		if (q > n - r)
		{
			std::size_t loopMin = std::max(p, n - r);
			for (std::size_t k = n + r - loopMin; k < 2 * r; ++k)
				integrand[k] = 0.0;
			for (std::size_t j = loopMin; j < q; ++j)
			{
				integrand[n + r - j] = 0.0;
				for (std::size_t k = 0; k < n + r - j; ++k)
				{
					integrand[k] = window[k] * signal[k + j - r];
				}
				dft(spectrogram[j], integrand, 2 * r);
			}
		}

	}


}
