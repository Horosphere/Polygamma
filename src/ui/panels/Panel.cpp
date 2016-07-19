#include "Panel.hpp"

#include <QMenu>
#include <QMainWindow>
#include <QCloseEvent>

namespace pg
{

Panel::Panel(QWidget* parent, Qt::WindowFlags flags):
	QDockWidget(parent, flags)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	setFocusPolicy(Qt::NoFocus);

	setAllowedAreas(Qt::AllDockWidgetAreas);
	connect(this, &Panel::customContextMenuRequested,
	        this, &Panel::onContextMenuRequest);
}
void Panel::show()
{
	setFloating(true);
	QWidget::show();
}
void Panel::closeEvent(QCloseEvent* event)
{
	hide();
	event->ignore();
}
void Panel::addDockActions(QMenu* const menu)
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
void Panel::onContextMenuRequest(QPoint point)
{
	QMenu* contextMenu = new QMenu(this);
	addDockActions(contextMenu);
	contextMenu->exec(mapToGlobal(point));
}

void Panel::onDockLeft()
{
	setFloating(false);
	((QMainWindow*) parent())->addDockWidget(Qt::LeftDockWidgetArea, this);
}
void Panel::onDockRight()
{
	setFloating(false);
	((QMainWindow*) parent())->addDockWidget(Qt::RightDockWidgetArea, this);
}
void Panel::onDockTop()
{
	setFloating(false);
	((QMainWindow*) parent())->addDockWidget(Qt::TopDockWidgetArea, this);
}
void Panel::onDockBottom()
{
	setFloating(false);
	((QMainWindow*) parent())->addDockWidget(Qt::BottomDockWidgetArea, this);
}

} // namespace pg
