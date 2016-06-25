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
public:
	Waveform(BufferSingular const* const buffer, std::size_t channelId, QWidget* parent = 0);

	QColor colourBackground;
	void setColourEdge(QColor);
	void setColourCore(QColor);
protected:
	void paintEvent(QPaintEvent*);

private:
	BufferSingular const* const buffer;
	std::size_t channelId;
	Vector<real> const* const channel;
	
	QPen penEdge;
	QPen penCore;
};

} // namespace pg


// Implementations

inline void pg::Waveform::setColourEdge(QColor colour)
{
	penEdge = QPen(colour, 1);
}
inline void pg::Waveform::setColourCore(QColor colour)
{
	penCore = QPen(colour, 1);
}
#endif // !_POLYGAMMA_UI_GRAPHICS_WAVEFORM_HPP__

