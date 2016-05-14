#ifndef WAVEFORM_HPP
#define WAVEFORM_HPP

#include "viewport2.hpp"

namespace pg
{

class Waveform final: public Viewport2
{
	Q_OBJECT
public:
	Waveform(QWidget* parent = 0);

protected:
	void paintEvent(QPaintEvent*);
};

}
#endif // WAVEFORM_HPP
