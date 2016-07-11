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

// ActionDialog
pg::ActionDialog::ActionDialog(DialogScriptArgs* const dialog, QObject* parent):
	ActionFlagged(parent), dialog(dialog)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		if (this->dialog->exec())
			Q_EMIT execute(this->dialog->script());
	});
}
pg::ActionDialog::ActionDialog(DialogScriptArgs* const dialog,
                               QString const& text, QObject* parent):
	ActionFlagged(text, parent), dialog(dialog)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		if (this->dialog->exec())
			Q_EMIT execute(this->dialog->script());
	});
}
pg::ActionDialog::ActionDialog(DialogScriptArgs* const dialog,
                               QIcon const& icon, QString const& text,
                               QObject* parent):
	ActionFlagged(icon, text, parent), dialog(dialog)
{
	connect(this, &QAction::triggered,
	        this, [this]()
	{
		if (this->dialog->exec())
			Q_EMIT execute(this->dialog->script());
	});
}
