#include "Terminal.hpp"

#include <QSplitter>
#include <QAction>
#include <QMenu>
#include <QMenuBar>
#include <QFont>
#include <QDebug>

pg::TerminalLog::TerminalLog(QWidget* parent): QPlainTextEdit(parent)
{
	setReadOnly(true);
}
pg::TerminalInput::TerminalInput(QWidget* parent): QPlainTextEdit(parent)
{
	// Required for detecting key combinations
	setFocusPolicy(Qt::StrongFocus);
}

void pg::TerminalLog::onLogUpdate(QString log)
{
	appendPlainText(log);
}

void pg::TerminalInput::keyPressEvent(QKeyEvent* event)
{
	if ((event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) &&
			event->modifiers() == Qt::ShiftModifier)
	{
		Q_EMIT execute(Command(document()->toPlainText().toStdString()));
		this->document()->setPlainText("");
	}
	else
	{
		QPlainTextEdit::keyPressEvent(event);
	}
}

pg::Terminal::Terminal(Kernel* const kernel,
                       QWidget* parent): QMainWindow(parent),
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
	QAction* actionLoadScript = new QAction("Load Script...", this);
	menuFile->addAction(actionLoadScript);

	// Signals
	auto logListener = [this](std::string c)
	{
		this->onLogUpdate(c);
	};
	kernel->registerLogListener(logListener);
	connect(input, &TerminalInput::execute,
	        this, &Terminal::onExecution);
	connect(this, &Terminal::logUpdate,
			log, &TerminalLog::onLogUpdate, Qt::QueuedConnection);

	// Default
	setBaseSize(QSize(300,500));

	QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	setFont(font);
}

void pg::Terminal::onExecution(Command const& c)
{
	kernel->pushCommand(c);
}

void pg::Terminal::onLogUpdate(std::string c)
{
	Q_EMIT logUpdate(QString::fromStdString(c));
}