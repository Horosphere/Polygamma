#include "PanelPlayback.hpp"

#include <QHBoxLayout>
#include <QPushButton>

namespace pg
{

PanelPlayback::PanelPlayback(QWidget* parent): Panel(parent),
	buttonPlayPause(new QPushButton)
{
	setWindowTitle(tr("Playback"));
	static QSize const buttonSize(64, 64);

	QHBoxLayout* layoutMain = new QHBoxLayout;
	QWidget* centralWidget = new QWidget;
	setWidget(centralWidget);
	centralWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	centralWidget->setLayout(layoutMain);

	buttonPlayPause->setIcon(QIcon(":/play.png"));
	buttonPlayPause->setIconSize(buttonSize);
	buttonPlayPause->setFixedSize(buttonSize);
	layoutMain->addWidget(buttonPlayPause);
	addSubwidget(buttonPlayPause);

	QPushButton* buttonStop = new QPushButton;
	buttonStop->setIcon(QIcon(":/stop.png"));
	buttonStop->setIconSize(buttonSize);
	buttonStop->setFixedSize(buttonSize);
	layoutMain->addWidget(buttonStop);
	addSubwidget(buttonStop);

	connect(buttonPlayPause, &QPushButton::clicked,
	        this, &PanelPlayback::playPause);
	connect(buttonStop, &QPushButton::clicked,
	        this, &PanelPlayback::stop);
};


} // namespace pg
