#ifndef _POLYGAMMA_UI_UI_HPP__
#define _POLYGAMMA_UI_UI_HPP__

#include <cstdint>

#include <QColor>
#include <QString>

#include "../core/Configuration.hpp"

namespace pg
{

QColor abgrToQColor(Configuration::Colour32);
Configuration::Colour32 qColorToABGR(QColor);
QString abgrToString(Configuration::Colour32);

/**
 * This is set to 1 since fixed point numbers don't work very well with Ticked
 * Axis (ui/util/Axis.hpp)
 *
 * Polygamma implements audio display using fixed-point numbers. The
 * value 1/UI_SAMPLE_DISPLAY_WIDTH represents the smallest number while drawing.
 * The fixed-point decimal is emulated.
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
constexpr int64_t const UI_SAMPLE_DISPLAY_WIDTH = 1;

constexpr int const UI_EVENTLOOP_INTERVAL = 10;

// Implementations

inline QColor abgrToQColor(Configuration::Colour32 c)
{
	// & has lower precedence compared to >>
	return QColor(c & 0xFF, c >> 8 & 0xFF, c >> 16 & 0xFF, c >> 24);
}

inline Configuration::Colour32 qColorToABGR(QColor c)
{
	return (c.red() & 0xFF) |
	       (c.green() & 0xFF) << 8 |
	       (c.blue() & 0xFF) << 16 |
	       (c.alpha() & 0xFf) << 24;
}
inline QString abgrToString(Configuration::Colour32 c)
{
	return abgrToQColor(c).name();
}

} // namespace pg

#endif // !_POLYGAMMA_UI_UI_HPP__

