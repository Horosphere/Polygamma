#include "mainWindowAccessories.hpp"

#include <QKeyEvent>

// ScriptAction
pg::ScriptAction::ScriptAction(Script const& command, QObject* parent):
	QAction(parent), command(command)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		Q_EMIT execute(this->command);
	});
}
pg::ScriptAction::ScriptAction(Script const& command, QString const& text,
                                 QObject* parent):
	QAction(text, parent), command(command)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		Q_EMIT execute(this->command);
	});
}
pg::ScriptAction::ScriptAction(Script const& command, QIcon const& icon,
                                 QString const& text, QObject* parent):
	QAction(icon, text, parent), command(command)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		Q_EMIT execute(this->command);
	});
}

// LineEditScript
pg::LineEditScript::LineEditScript(QWidget* parent): QLineEdit(parent)
{
}
void pg::LineEditScript::keyPressEvent(QKeyEvent* event)
{
	if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) &&
	        event->modifiers() == Qt::ShiftModifier)
	{
		Q_EMIT execute(Script(text().toStdString()));
		this->clear();
	}
	else
	{
		QLineEdit::keyPressEvent(event);
	}
}
