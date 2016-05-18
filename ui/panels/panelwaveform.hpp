#ifndef PANELWAVEFORM_HPP
#define PANELWAVEFORM_HPP

#include "panelbase.hpp"

namespace pg
{

class PanelWaveform final: public PanelBase
{
	Q_OBJECT
public:
	explicit PanelWaveform(QString title, QWidget* parent = 0);

private:
	Waveform* waveform;
};

}
#endif // PANELWAVEFORM_HPP
