#include "PanelBase.hpp"

#include <QMenu>
#include <QMainWindow>

pg::PanelBase::PanelBase(QWidget* parent, Qt::WindowFlags flags):
	QDockWidget(parent, flags)
{
	setContextMenuPolicy(Qt::CustomContextMenu);
	setFocusPolicy(Qt::NoFocus);

	setAllowedAreas(Qt::AllDockWidgetAreas);
	connect(this, &PanelBase::customContextMenuRequested,
			this, &PanelBase::onContextMenuRequest);
}

void pg::PanelBase::addDockActions(QMenu* const menu)
{
	QAction* action = new QAction("Dock left", this);
	connect(action, &QAction::triggered,
			this, &PanelBase::onDockLeft);
	menu->addAction(action);
	action = new QAction("Dock right", this);
	connect(action, &QAction::triggered,
			this, &PanelBase::onDockRight);
	menu->addAction(action);
	action = new QAction("Dock top", this);
	connect(action, &QAction::triggered,
			this, &PanelBase::onDockTop);
	menu->addAction(action);
	action = new QAction("Dock bottom", this);
	connect(action, &QAction::triggered,
			this, &PanelBase::onDockBottom);
	menu->addAction(action);

}
void pg::PanelBase::onContextMenuRequest(QPoint point)
{
	QMenu* contextMenu = new QMenu(this);
	addDockActions(contextMenu);
	contextMenu->exec(mapToGlobal(point));
}

void pg::PanelBase::onDockLeft()
{
	setFloating(false);
	((QMainWindow*) parent())->addDockWidget(Qt::LeftDockWidgetArea, this);
}
void pg::PanelBase::onDockRight()
{
	setFloating(false);
	((QMainWindow*) parent())->addDockWidget(Qt::RightDockWidgetArea, this);
}
void pg::PanelBase::onDockTop()
{
	setFloating(false);
	((QMainWindow*) parent())->addDockWidget(Qt::TopDockWidgetArea, this);
}
void pg::PanelBase::onDockBottom()
{
	setFloating(false);
	((QMainWindow*) parent())->addDockWidget(Qt::BottomDockWidgetArea, this);
}
