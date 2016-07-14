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
	void playPause();
	void stop();

public Q_SLOTS:
	void setPlay(); // Set the playpause button to play
	void setPause(); // Set the playpause button to pause

private:
	QPushButton* buttonPlayPause;
};

} // namespace pg

// Implementations

inline void pg::PanelPlayback::setPlay()
{
	buttonPlayPause->setIcon(QIcon(":/play.png"));
}
inline void pg::PanelPlayback::setPause()
{
	buttonPlayPause->setIcon(QIcon(":/pause.png"));
}
#endif // !_POLYGAMMA_UI_PANELS_PANELPLAYBACK_HPP__
