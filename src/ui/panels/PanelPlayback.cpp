#include "PanelPlayback.hpp"

#include <QHBoxLayout>
#include <QPushButton>

namespace pg
{

PanelPlayback::PanelPlayback(QWidget* parent): Panel(parent),
	buttonPlayPause(new QPushButton)
{
	setWindowTitle("Playback");
	static QSize const buttonSize(64, 64);

	QWidget* centralWidget = new QWidget;
	setWidget(centralWidget);
	QHBoxLayout* layoutMain = new QHBoxLayout;
	centralWidget->setLayout(layoutMain);

	buttonPlayPause->setIcon(QIcon(":/play.png"));
	buttonPlayPause->setIconSize(buttonSize);
	buttonPlayPause->setFixedSize(buttonSize);
	layoutMain->addWidget(buttonPlayPause);

	QPushButton* buttonStop = new QPushButton;
	buttonStop->setIcon(QIcon(":/stop.png"));
	buttonStop->setIconSize(buttonSize);
	buttonStop->setFixedSize(buttonSize);
	layoutMain->addWidget(buttonStop);

	centralWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	connect(buttonPlayPause, &QPushButton::clicked,
	        this, &PanelPlayback::playPause);
	connect(buttonStop, &QPushButton::clicked,
	        this, &PanelPlayback::stop);
};


} // namespace pg
