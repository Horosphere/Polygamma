#include "LineEditCommand.hpp"

#include <QKeyEvent>

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
