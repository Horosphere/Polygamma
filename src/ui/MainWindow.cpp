#include "MainWindow.hpp"

#include <exception>

#include <QApplication>
#include <QCloseEvent>
#include <QDebug>
#include <QDir>
#include <QFileDialog>
#include <QFont>
#include <QMessageBox>
#include <QPushButton>
#include <QResource>
#include <QVBoxLayout>

#include "DialogPreferences.hpp"
#include "Terminal.hpp"
#include "editors/EditorSingular.hpp"
#include "ui.hpp"
#include "util/actions.hpp"


pg::MainWindow::MainWindow(Kernel* const kernel, Configuration* const config
, QWidget* parent): QMainWindow(parent),
	kernel(kernel), config(config), terminal(new Terminal(kernel, this)),
	lineEditScript(new LineEditScript), lineEditLog(new QLineEdit),
	currentEditor(nullptr)
{
	setWindowIcon(QIcon(":/icon.png"));
	setDockOptions(dockOptions() |
	               QMainWindow::AnimatedDocks |
	               QMainWindow::AllowTabbedDocks |
	               QMainWindow::GroupedDragging);
	//setTabPosition(Qt::RightDockWidgetArea, QTabWidget::East);
	setWindowTitle("Polygamma");

	// Menus
	// Menu File
	QMenu* menuFile = menuBar()->addMenu(tr("File"));
	QAction* actionFileImport = new QAction(tr("Import..."), this);
	menuFile->addAction(actionFileImport);
	// Menu Edit
	QMenu* menuEdit = menuBar()->addMenu(tr("Edit"));
	ScriptAction* actionEditSummon = new ScriptAction("print('summon')", "Summon", this);
	connect(actionEditSummon, &ScriptAction::execute,
	        this, &MainWindow::onExecute);
	menuEdit->addAction(actionEditSummon);
	ScriptAction* actionEditSilence = new ScriptAction("pg.silence(pg.kernel.buffers[%1])", "Silence", this);
	connect(actionEditSilence, &ScriptAction::execute,
	        this, &MainWindow::onExecute);
	menuEdit->addAction(actionEditSilence);
	QAction* actionEditPreferences = new QAction(tr("Preferences..."), this);
	menuEdit->addAction(actionEditPreferences);
	// Menu end

	// Status bar
	statusBar()->addPermanentWidget(lineEditScript, 1);
	lineEditLog->setReadOnly(true);
	statusBar()->addPermanentWidget(lineEditLog, 1);
	QPushButton* buttonTerminal = new QPushButton;
	buttonTerminal->setIcon(QIcon(":/terminal.png"));
	buttonTerminal->setFixedSize(QSize(25,25));
	buttonTerminal->setIconSize(QSize(25,25));
	statusBar()->addPermanentWidget(buttonTerminal);
	statusBar()->show();

	// Connects the GUI.
	config->registerUpdateListener([this]()
	{
		this->updateUIElements();
	});
	connect(buttonTerminal, &QPushButton::clicked,
	        this, [this]()
	{
		this->terminal->show();
	});

	// Menu actions
	connect(actionFileImport, &QAction::triggered,
	        this, [this]()
	{
		QString fileName = QFileDialog::getOpenFileName(this, tr("Import..."));
		if (fileName.isNull()) return;
		else
			this->terminal->onExecute(Script(std::string(PYTHON_KERNEL) +
			                                  ".fromFileImport(\"" +
			                                  fileName.toStdString() + "\")"));

	});
	connect(actionEditPreferences, &QAction::triggered,
	        this, [this]()
	{
		// TODO: Make sure that only one instance of DialogPreferences can exist at
		// a time.
		(new DialogPreferences(this->config, this))->show();
	});

	// Connects the kernel's stdout and stderr to the corresponding signals of
	// this class
	kernel->registerStdOutListener([this](std::string str)
	{
		Q_EMIT stdOutFlush(QString::fromStdString(str));
	});
	kernel->registerStdErrListener([this](std::string str)
	{
		Q_EMIT stdErrFlush(QString::fromStdString(str));
	});
	kernel->registerBufferListener([this](Buffer* buffer)
	{
		Q_EMIT newBuffer(buffer);
	});
	connect(this, &MainWindow::stdOutFlush,
	        terminal->log, &TerminalLog::onStdOutFlush, Qt::QueuedConnection);
	connect(this, &MainWindow::stdErrFlush,
	        terminal->log, &TerminalLog::onStdErrFlush, Qt::QueuedConnection);
	// Script line
	connect(lineEditScript, &LineEditScript::execute,
	        terminal, &Terminal::onExecute);
	connect(this, &MainWindow::stdOutFlush,
	        this, [this](QString const& str)
	{
		QStringList lines = str.split('\n', QString::SkipEmptyParts);
		if (lines.isEmpty()) return;
		lineEditLog->setStyleSheet(lineEditLog_stylesheetOut);
		lineEditLog->setText(lines.takeLast());
	}, Qt::QueuedConnection);
	connect(this, &MainWindow::stdErrFlush,
	        this, [this](QString const& str)
	{
		QStringList lines = str.split('\n', QString::SkipEmptyParts);
		if (lines.isEmpty()) return;
		lineEditLog->setText(lines.takeLast());
		lineEditLog->setStyleSheet(lineEditLog_stylesheetErr);
	}, Qt::QueuedConnection);
	connect(this, &MainWindow::newBuffer,
	        this, &MainWindow::onNewBuffer, Qt::QueuedConnection);

	updateUIElements();

	// Set default states
	setBaseSize(QSize(300, 500));
	lineEditLog->setStyleSheet(lineEditLog_stylesheetOut);
	terminal->setBaseSize(QSize(300, 500));
}

void pg::MainWindow::closeEvent(QCloseEvent* event)
{
	kernel->halt();
	QMainWindow::closeEvent(event);
}

void pg::MainWindow::updateUIElements()
{
	// Load
	QFont fontMonospace = QFontDatabase::systemFont(QFontDatabase::FixedFont);
	int const tabWidth = 4 * QFontMetrics(fontMonospace).width(' ');

	// Apply
	lineEditScript->setFont(fontMonospace);
	lineEditLog->setFont(fontMonospace);
	lineEditLog_stylesheetOut = QString("color: white; background-color: black");
	lineEditLog_stylesheetErr = QString("color: white; background-color: #220000");

	terminal->log->setFont(fontMonospace);
	terminal->log->setTabStopWidth(tabWidth);
	terminal->input->setFont(fontMonospace);
	terminal->input->setTabStopWidth(tabWidth);

	/*
	 * "Bool To String" Converts a bool to a QString for using the stylesheet.
	 */
	auto bts = [](bool b) -> QString { return b ? "true" : "false"; };
	qApp->setStyleSheet(
	  "QMainWindow, QDialog, QDockWidget, QStatusBar {"
	  "background-color: " + abgrToString(config->uiBG) + ";"
	  "}"
	  "pg--Terminal {"
	  "qproperty-showSystemLevel: " + bts(config->uiTerminalShowSystemLevel) + ";"
	  "}"
	  "pg--Waveform { "
	  "qproperty-colourBG: " + abgrToString(config->uiWaveformBG) + ";"
	  "qproperty-colourCore: " + abgrToString(config->uiWaveformCore) +";"
	  "qproperty-colourEdge: " + abgrToString(config->uiWaveformEdge) + ";"
	  "}");
}

void pg::MainWindow::onNewBuffer(Buffer* buffer)
{
	switch (buffer->getType())
	{
	case Buffer::Singular:
		qDebug() << "[UI] BufferSingular detected";
		currentEditor = new EditorSingular(kernel, (BufferSingular*) buffer, this);
		break;
	default:
		qDebug() << "[UI] Unrecognised Buffer Type";
		throw std::logic_error("Buffer Type " +
		                       std::to_string((int)buffer->getType()) +
		                       "is not recognised by MainWindow");
	}
	currentEditor->show();
	connect(currentEditor, &Editor::execute,
	        terminal, &Terminal::onExecute);
	connect(currentEditor, &Editor::editorClose,
			this, [kernel = this->kernel, buffer]
			{
			kernel->pushSpecial(Kernel::Special{Kernel::Special::Deletion, {buffer}});
			});
	
}

void pg::MainWindow::onExecute(QString const& script)
{
	assert(currentEditor != nullptr);

	std::size_t index = kernel->bufferIndex(currentEditor->getBuffer());

	terminal->onExecute(Script(script.arg(index).toStdString()));
}

