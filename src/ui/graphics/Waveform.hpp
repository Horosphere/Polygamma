#ifndef _POLYGAMMA_UI_GRAPHICS_WAVEFORM_HPP__
#define _POLYGAMMA_UI_GRAPHICS_WAVEFORM_HPP__

#include <QPen>

#include "Viewport2.hpp"
#include "../../singular/BufferSingular.hpp"

namespace pg
{

class Waveform final: public Viewport2
{
	Q_OBJECT

	Q_PROPERTY(QColor colourBG READ getColourBG WRITE setColourBG)
	Q_PROPERTY(QColor colourCore READ getColourCore WRITE setColourCore)
	Q_PROPERTY(QColor colourEdge READ getColourEdge WRITE setColourEdge)
public:
	Waveform(BufferSingular const* const buffer, std::size_t channelId, QWidget* parent = 0);

	void setColourBG(QColor) noexcept;
	QColor getColourBG() const noexcept;
	void setColourCore(QColor) noexcept;
	QColor getColourCore() const noexcept;
	void setColourEdge(QColor) noexcept;
	QColor getColourEdge() const noexcept;
	
protected:
	void paintEvent(QPaintEvent*);

private:
	QPen penCore;
	QPen penEdge;
	QColor colourBackground;

	BufferSingular const* const buffer;
	std::size_t channelId;
	Vector<real> const* const channel;
	
};

} // namespace pg


// Implementations

inline void pg::Waveform::setColourBG(QColor colour) noexcept
{
	colourBackground = colour;
}
inline QColor pg::Waveform::getColourBG() const noexcept
{
	return colourBackground;
}
inline void pg::Waveform::setColourCore(QColor colour) noexcept
{
	penCore = QPen(colour, 1);
}
inline QColor pg::Waveform::getColourCore() const noexcept
{
	return penCore.color();
}
inline void pg::Waveform::setColourEdge(QColor colour) noexcept
{
	penEdge = QPen(colour, 1);
}
inline QColor pg::Waveform::getColourEdge() const noexcept
{
	return penEdge.color();
}
#endif // !_POLYGAMMA_UI_GRAPHICS_WAVEFORM_HPP__

