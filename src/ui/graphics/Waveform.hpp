#ifndef _POLYGAMMA_UI_GRAPHICS_WAVEFORM_HPP__
#define _POLYGAMMA_UI_GRAPHICS_WAVEFORM_HPP__

#include "Viewport2.hpp"

#include "../../math/Audio.hpp"

namespace pg
{

class Waveform final: public Viewport2
{
	Q_OBJECT
public:
	Waveform(QWidget* parent = 0);

	void setChannel(Audio::Channel const* const channel);

protected:
	void paintEvent(QPaintEvent*);

private:
	Audio::Channel const* channel;
};

} // namespace pg


#endif // !_POLYGAMMA_UI_GRAPHICS_WAVEFORM_HPP__

