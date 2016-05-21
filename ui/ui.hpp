#ifndef UI_HPP
#define UI_HPP

#include <cstdint>

namespace pg
{

/**
 * Polygamma implements audio display using fixed-point floating points. The
 * value 1/UI_SAMPLE_DISPLAY_WIDTH represents the smallest number while drawing.
 * The fixed-point floating point is emulated.
 *
 * While drawing waveforms, the "ranges" (represented by Interval<int64_t>s) are
 * controlled so that they always have greater or equal length comparing to the
 * corresponding UI dimension.
 *
 * @warning This number inverse proportionally thresholds the longest audio
 *     Polygamma can display. It is best to keep it under 64.
 * @brief UI_SAMPLE_DISPLAY_WIDTH The width in pixels between two samples while
 *     plotting audio.
 */
constexpr int64_t const UI_SAMPLE_DISPLAY_WIDTH = 32;

} // namespace pg
#endif // UI_HPP
