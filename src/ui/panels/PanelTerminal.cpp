#include "PanelTerminal.hpp"

#include <QVBoxLayout>
#include <QKeyEvent>

pg::TerminalLog::TerminalLog(QWidget *parent): QTextEdit(parent)
{

}

pg::TerminalInput::TerminalInput(QWidget *parent): QPlainTextEdit(parent)
{

}

void pg::TerminalInput::keyPressEvent(QKeyEvent* event)
{
	if (event->key() == Qt::Key_Enter &&
			event->modifiers() == Qt::ShiftModifier)
	{
		Q_EMIT execute(document()->toPlainText().toStdString());
		document()->clear();
	}
}

pg::PanelTerminal::PanelTerminal(QWidget *parent) : PanelBase(parent),
	log(new TerminalLog(this)), input(new TerminalInput(this))
{
	QVBoxLayout* mainLayout = new QVBoxLayout(this);
	mainLayout->addWidget(log);
	mainLayout->addWidget(input);
}
