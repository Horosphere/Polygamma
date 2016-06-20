#include "mainWindowAccessories.hpp"

#include <QKeyEvent>

// CommandAction
pg::CommandAction::CommandAction(Command const& command, QObject* parent):
	QAction(parent), command(command)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		Q_EMIT execute(this->command);
	});
}
pg::CommandAction::CommandAction(Command const& command, QString const& text,
                                 QObject* parent):
	QAction(text, parent), command(command)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		Q_EMIT execute(this->command);
	});
}
pg::CommandAction::CommandAction(Command const& command, QIcon const& icon,
                                 QString const& text, QObject* parent):
	QAction(icon, text, parent), command(command)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		Q_EMIT execute(this->command);
	});
}

// LineEditCommand
pg::LineEditCommand::LineEditCommand(QWidget* parent): QLineEdit(parent)
{
}
void pg::LineEditCommand::keyPressEvent(QKeyEvent* event)
{
	if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) &&
	        event->modifiers() == Qt::ShiftModifier)
	{
		Q_EMIT execute(Command(text().toStdString()));
		this->clear();
	}
	else
	{
		QLineEdit::keyPressEvent(event);
	}
}
