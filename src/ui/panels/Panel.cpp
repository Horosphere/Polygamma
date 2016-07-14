#include "Panel.hpp"

#include <QMenu>
#include <QMainWindow>
#include <QCloseEvent>

pg::Panel::Panel(QWidget* parent, Qt::WindowFlags flags):
	QDockWidget(parent, flags)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	setFocusPolicy(Qt::NoFocus);

	setAllowedAreas(Qt::AllDockWidgetAreas);
	connect(this, &Panel::customContextMenuRequested,
			this, &Panel::onContextMenuRequest);
}
void pg::Panel::show()
{
	setFloating(true);
	QWidget::show();
}
void pg::Panel::closeEvent(QCloseEvent* event)
{
	hide();
	event->ignore();
}
void pg::Panel::addDockActions(QMenu* const menu)
{
	QAction* action = new QAction("Dock left", this);
	connect(action, &QAction::triggered,
			this, &Panel::onDockLeft);
	menu->addAction(action);
	action = new QAction("Dock right", this);
	connect(action, &QAction::triggered,
			this, &Panel::onDockRight);
	menu->addAction(action);
	action = new QAction("Dock top", this);
	connect(action, &QAction::triggered,
			this, &Panel::onDockTop);
	menu->addAction(action);
	action = new QAction("Dock bottom", this);
	connect(action, &QAction::triggered,
			this, &Panel::onDockBottom);
	menu->addAction(action);

}
void pg::Panel::onContextMenuRequest(QPoint point)
{
	QMenu* contextMenu = new QMenu(this);
	addDockActions(contextMenu);
	contextMenu->exec(mapToGlobal(point));
}

void pg::Panel::onDockLeft()
{
	setFloating(false);
	((QMainWindow*) parent())->addDockWidget(Qt::LeftDockWidgetArea, this);
}
void pg::Panel::onDockRight()
{
	setFloating(false);
	((QMainWindow*) parent())->addDockWidget(Qt::RightDockWidgetArea, this);
}
void pg::Panel::onDockTop()
{
	setFloating(false);
	((QMainWindow*) parent())->addDockWidget(Qt::TopDockWidgetArea, this);
}
void pg::Panel::onDockBottom()
{
	setFloating(false);
	((QMainWindow*) parent())->addDockWidget(Qt::BottomDockWidgetArea, this);
}
