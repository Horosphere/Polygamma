#include "Terminal.hpp"

#include <QSplitter>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFont>
#include <QDebug>
#include <QCloseEvent>

#include "util/SyntaxHighlighterPython.hpp"

// Terminal Log
pg::TerminalLog::TerminalLog(QWidget* parent): QTextEdit(parent)
{
	setReadOnly(true);
}

void pg::TerminalLog::onStdOutFlush(QString str)
{
	moveCursor(QTextCursor::End);
	insertHtml("<span style=\"color:#000000;\">" + str.toHtmlEscaped().replace('\n', "<br>") + "</span>");
	moveCursor(QTextCursor::End);
}
void pg::TerminalLog::onStdErrFlush(QString str)
{
	moveCursor(QTextCursor::End);
	insertHtml("<span style=\"color:#FF0000;\">" + str.toHtmlEscaped().replace('\n', "<br>") + "</span>");
	moveCursor(QTextCursor::End);
}

// Terminal Input
pg::TerminalInput::TerminalInput(QWidget* parent): QPlainTextEdit(parent),
	preserveInput(true)
{
	// Required for detecting key combinations
	setFocusPolicy(Qt::StrongFocus);

	new SyntaxHighlighterPython(document());
}

void pg::TerminalInput::keyPressEvent(QKeyEvent* event)
{
	// Shift + Enter executes the command
	if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) &&
	    event->modifiers() == Qt::ShiftModifier)
	{
		Q_EMIT execute(Script(document()->toPlainText().toStdString()));
		if (!preserveInput)
			this->document()->setPlainText("");
	}
	else
	{
		QPlainTextEdit::keyPressEvent(event);
	}
}
void pg::TerminalInput::onPreserveInputToggled(bool checked)
{
	preserveInput = checked;
}

// Terminal
pg::Terminal::Terminal(Kernel* const kernel, QWidget* parent):
	QMainWindow(parent),
	log(new TerminalLog), input(new TerminalInput),
	kernel(kernel)
{
	setWindowTitle(tr("Terminal"));
	// Register layout items
	QSplitter* layoutMain = new QSplitter(Qt::Vertical, this);
	setCentralWidget(layoutMain);

	layoutMain->addWidget(log);
	layoutMain->addWidget(input);

	// Menues
	QMenu* menuFile = menuBar()->addMenu(tr("File"));
	QAction* actionLoadScript = new QAction(tr("Load Script..."), this);
	menuFile->addAction(actionLoadScript);
	QMenu* menuEdit = menuBar()->addMenu(tr("Edit"));
	QAction* actionPreserveInput = new QAction(tr("Preserve Input"), this);
	actionPreserveInput->setCheckable(true);
	menuEdit->addAction(actionPreserveInput);

	// Signals
	connect(input, &TerminalInput::execute,
	        this, &Terminal::onExecute);
	connect(actionPreserveInput, &QAction::toggled,
	        input, &TerminalInput::onPreserveInputToggled);
}

void pg::Terminal::closeEvent(QCloseEvent* event)
{
	this->hide();
	event->ignore();
}
void pg::Terminal::onExecute(Script const& script)
{
	if (!script.isSystemLevel() || showSystemLevel)
		log->onStdOutFlush(QString::fromStdString("\n>> " + (std::string)script + "\n"));
	this->kernel->pushScript(script);
}

