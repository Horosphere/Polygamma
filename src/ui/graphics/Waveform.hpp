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

	Q_PROPERTY(QColor colourCore READ getColourCore WRITE setColourCore)
	Q_PROPERTY(QColor colourEdge READ getColourEdge WRITE setColourEdge)

	QPen penCore;
	QPen penEdge;
public:
	Waveform(BufferSingular const* const buffer, std::size_t channelId,
			QWidget* parent = 0);

	void setColourCore(QColor) noexcept;
	QColor getColourCore() const noexcept;
	void setColourEdge(QColor) noexcept;
	QColor getColourEdge() const noexcept;
	
Q_SIGNALS:
	void cursorMove(std::size_t);

protected:
	void paintEvent(QPaintEvent*);

private:
	BufferSingular const* const buffer;
	std::size_t channelId;
	Vector<real> const* const channel;
};

} // namespace pg


// Implementations

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

