#include "Terminal.hpp"

#include <QSplitter>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFont>
#include <QDebug>
#include <QCloseEvent>
#include <QTimer>

#include "util/SyntaxHighlighterPython.hpp"
#include "ui.hpp"

namespace pg
{

// Terminal Log
TerminalLog::TerminalLog(QWidget* parent): QTextEdit(parent)
{
	setReadOnly(true);
}

void TerminalLog::onStdOutFlush(QString str)
{
	moveCursor(QTextCursor::End);
	insertHtml("<span style=\"color:#000000;\">" + str.replace('\n', "<br>") + "</span>");
	moveCursor(QTextCursor::End);
}
void TerminalLog::onStdErrFlush(QString str)
{
	moveCursor(QTextCursor::End);
	insertHtml("<span style=\"color:#FF0000;\">" + str.replace('\n', "<br>") + "</span>");
	moveCursor(QTextCursor::End);
}

// Terminal Input
TerminalInput::TerminalInput(QWidget* parent): QPlainTextEdit(parent),
	preserveInput(true)
{
	// Required for detecting key combinations
	setFocusPolicy(Qt::StrongFocus);

	new SyntaxHighlighterPython(document());
}

void TerminalInput::keyPressEvent(QKeyEvent* event)
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
void TerminalInput::onPreserveInputToggled(bool checked)
{
	preserveInput = checked;
}

// Terminal
Terminal::Terminal(Kernel* const kernel, QWidget* parent):
	QMainWindow(parent),
	log(new TerminalLog), input(new TerminalInput), kernel(kernel),
	acceptingScripts(true)
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
	connect(this, &Terminal::stdOutFlush, log, &TerminalLog::onStdOutFlush);
	connect(this, &Terminal::stdErrFlush, log, &TerminalLog::onStdErrFlush);

	// Event loop
	QTimer* timer = new QTimer(this);
	timer->setInterval(UI_EVENTLOOP_INTERVAL);
	timer->setSingleShot(false);
	connect(timer, &QTimer::timeout, this, &Terminal::eventLoop);
	timer->start();
}

void Terminal::onExecute(Script const& script)
{
	assert(acceptingScripts);
	if (script.level >= scriptLevelMin)
		log->onStdOutFlush(QString::fromStdString("\n>> " + (std::string)script + "\n"));
	kernel->execute(script);
	acceptingScripts = false;
}

void Terminal::eventLoop()
{
	Kernel::ScriptOutput scriptOutput;
	while (kernel->popScriptOutput(&scriptOutput))
	{
		if (scriptOutput.stream == Kernel::ScriptOutput::StdOut)
			Q_EMIT stdOutFlush(QString::fromStdString(scriptOutput.string));
		else
			Q_EMIT stdErrFlush(QString::fromStdString(scriptOutput.string));
	}
	Kernel::SpecialOutput specialOutput;
	while (kernel->popSpecialOutput(&specialOutput))
	{
		switch (specialOutput.type)
		{
		case Kernel::SpecialOutput::Completion:
			acceptingScripts = true;
			break;
		case Kernel::SpecialOutput::ConfigUpdate:
			Q_EMIT configUpdate();
			break;
		case Kernel::SpecialOutput::BufferNew:
			Q_EMIT bufferNew(specialOutput.buffer);
			break;
		case Kernel::SpecialOutput::BufferErase:
			Q_EMIT bufferErase(specialOutput.buffer);
			break;
		case Kernel::SpecialOutput::BufferUpdate:
			Q_EMIT bufferUpdate(specialOutput.buffer, specialOutput.update);
			break;
		default:
			assert(false && "Unrecognised SpecialOutput");
		}
	}
}

} // namespace pg

