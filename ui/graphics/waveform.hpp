#ifndef WAVEFORM_HPP
#define WAVEFORM_HPP

#include "viewport2.hpp"

#include "../../math/audio.hpp"

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


#endif // WAVEFORM_HPP
