#include "mainWindowAccessories.hpp"

#include <QKeyEvent>


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
