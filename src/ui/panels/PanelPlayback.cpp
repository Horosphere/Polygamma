#include "PanelPlayback.hpp"

#include <QHBoxLayout>
#include <QPushButton>

namespace pg
{

PanelPlayback::PanelPlayback(QWidget* parent): Panel(parent)
{
	setWindowTitle(tr("Playback"));
	static QSize const buttonSize(64, 64);

	QHBoxLayout* layoutMain = new QHBoxLayout;
	QWidget* centralWidget = new QWidget;
	setWidget(centralWidget);
	centralWidget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
	centralWidget->setLayout(layoutMain);

	QPushButton* buttonPlay = new QPushButton;
	buttonPlay->setIcon(QIcon(":/play.png"));
	buttonPlay->setIconSize(buttonSize);
	buttonPlay->setFixedSize(buttonSize);
	layoutMain->addWidget(buttonPlay);
	addSubwidget(buttonPlay);

	QPushButton* buttonPause = new QPushButton;
	buttonPause->setIcon(QIcon(":/pause.png"));
	buttonPause->setIconSize(buttonSize);
	buttonPause->setFixedSize(buttonSize);
	layoutMain->addWidget(buttonPause);
	addSubwidget(buttonPause);

	QPushButton* buttonStop = new QPushButton;
	buttonStop->setIcon(QIcon(":/stop.png"));
	buttonStop->setIconSize(buttonSize);
	buttonStop->setFixedSize(buttonSize);
	layoutMain->addWidget(buttonStop);
	addSubwidget(buttonStop);

	connect(buttonPlay, &QPushButton::clicked,
	        this, &PanelPlayback::play);
	connect(buttonPause, &QPushButton::clicked,
	        this, &PanelPlayback::pause);
	connect(buttonStop, &QPushButton::clicked,
	        this, &PanelPlayback::stop);
};


} // namespace pg
