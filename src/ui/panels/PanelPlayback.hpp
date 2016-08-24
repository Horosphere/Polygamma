#ifndef _POLYGAMMA_UI_PANELS_PANELPLAYBACK_HPP__
#define _POLYGAMMA_UI_PANELS_PANELPLAYBACK_HPP__

#include <QPushButton>

#include "Panel.hpp"

namespace pg
{

class PanelPlayback final: public Panel
{
	Q_OBJECT

public:
	explicit PanelPlayback(QWidget* parent = 0);

Q_SIGNALS:
	void play();
	void pause();
	void stop();

};


} // namespace pg

#endif // !_POLYGAMMA_UI_PANELS_PANELPLAYBACK_HPP__
