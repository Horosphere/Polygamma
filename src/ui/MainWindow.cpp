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

#include "ui.hpp"
#include "Terminal.hpp"
#include "dialogs/DialogPreferences.hpp"
#include "dialogs/DialogNewSingular.hpp"
#include "editors/EditorSingular.hpp"


pg::MainWindow::MainWindow(Kernel* const kernel, Configuration* const config
, QWidget* parent): QMainWindow(parent),
	kernel(kernel), config(config), terminal(new Terminal(kernel, this)),
	lineEditScript(new LineEditScript), lineEditLog(new QLineEdit),
	currentEditor(nullptr)
{
	setFocusPolicy(Qt::NoFocus);
	setWindowIcon(QIcon(":/icon.png"));
	setDockOptions(dockOptions() |
	               QMainWindow::AnimatedDocks |
	               QMainWindow::AllowTabbedDocks |
	               QMainWindow::GroupedDragging);
	//setTabPosition(Qt::RightDockWidgetArea, QTabWidget::East);
	setWindowTitle("Polygamma");

// Arguments: menu, action, flags
// Call this if the given scriptAction should be deactivated
#define ADD_ACTIONFLAGGED(menu, action, fl) \
	action->flags = fl; \
	actionsFlagged.push_back(action); \
	menu->addAction(action)
#define ADD_ACTIONSCRIPTED(menu, action, fl) \
	action->flags = fl; \
	connect(action, &ActionScripted::execute, \
	        this, &MainWindow::onExecute); \
	actionsFlagged.push_back(action); \
	menu->addAction(action)

	// Menus
	// Menu File
	QMenu* menuFile = menuBar()->addMenu(tr("File"));

	// Menu File -> New
	QMenu* menuFileNew = menuFile->addMenu(tr("New"));
	QAction* actionFileNewSingular = new QAction(tr("Singular..."), this);
	menuFileNew->addAction(actionFileNewSingular);
	QAction* actionFileImport = new QAction(tr("Import..."), this);
	menuFile->addAction(actionFileImport);
	ActionFlagged* actionFileSaveAs = new ActionFlagged(tr("Save As..."), this);
	ADD_ACTIONFLAGGED(menuFile, actionFileSaveAs, ActionFlagged::FLAG_FULL);

	// Menu Edit
	QMenu* menuEdit = menuBar()->addMenu(tr("Edit"));
	ActionScripted* actionEditSummon = new ActionScripted("print('summon')", "Summon", this);
	connect(actionEditSummon, &ActionScripted::execute,
	        this, &MainWindow::onExecute);
	menuEdit->addAction(actionEditSummon);

	ActionScripted* actionEditSilence = new ActionScripted("pg.silence(%1)", "Silence", this);
	ADD_ACTIONSCRIPTED(menuEdit, actionEditSilence, Buffer::Singular);
	QAction* actionEditPreferences = new QAction(tr("Preferences..."), this);
	menuEdit->addAction(actionEditPreferences);
	// Menu end
#undef ADD_ACTIONFLAGGED
#undef ADD_ACTIONSCRIPTED

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
	connect(qApp, &QApplication::focusChanged,
	        this, &MainWindow::onFocusChanged);
	connect(buttonTerminal, &QPushButton::clicked,
	        this, [this]()
	{
		this->terminal->show();
	});

	// Menu actions
	connect(actionFileNewSingular, &QAction::triggered,
	        this, [this]()
	{
		DialogNewSingular* dialog = new DialogNewSingular(this);
		if (dialog->exec())
		{
			auto values = dialog->values();
			std::string script = std::string(PYTHON_KERNEL) + ".createSingular(" +
			                     std::to_string(std::get<0>(values)) + ", " +
			                     std::to_string(std::get<1>(values)) + ", \"" +
			                     std::get<2>(values) + "\")";
			this->terminal->onExecute(Script(script));
		}
	});
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
	connect(actionFileSaveAs, &QAction::triggered,
	        this, [this]()
	{
		QString fileName = QFileDialog::getSaveFileName(this, tr("Import..."));
		if (fileName.isNull()) return;
		else
			this->onExecute(QString(PYTHON_KERNEL) + ".buffers[%1].saveToFile('" +
			                fileName + "')");
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
	// Disable all actions since nothing is loaded
	for (auto& action: actionsFlagged) action->setEnabled(false);
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
	static auto bts = [](bool b) -> QString { return b ? "true" : "false"; };
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
	Editor* editor;

	switch (buffer->getType())
	{
	case Buffer::Singular:
		qDebug() << "[UI] BufferSingular detected";
		editor = new EditorSingular(kernel, (BufferSingular*) buffer, this);
		break;
	default:
		qDebug() << "[UI] Unrecognised Buffer Type";
		throw std::logic_error("Buffer Type " +
		                       std::to_string((int)buffer->getType()) +
		                       "is not recognised by MainWindow");
	}
	editor->show();
	editor->setFloating(true);
	currentEditor = editor;

	connect(editor, &Editor::execute,
	        terminal, &Terminal::onExecute);
}

void pg::MainWindow::onExecute(QString const& script)
{
	if (currentEditor)
	{
		std::size_t index = kernel->bufferIndex(currentEditor->getBuffer());
		QString s = script.arg(QString(PYTHON_KERNEL) + '(' +
		                       QString::number(index) + ')');

		terminal->onExecute(Script(s.toStdString()));
	}
	else
		qDebug() << "[UI] Current editor null but command is called: "
		         << script;
}

void pg::MainWindow::onFocusChanged(QWidget* old, QWidget* now)
{
	(void) old;
	if (Editor* editor = dynamic_cast<Editor*>(now))
	{
//		qDebug() << "[UI] Focus changed to new editor";
		currentEditor = editor;
		Buffer::Type type = editor->getBuffer()->getType();
		for (auto& action: actionsFlagged)
		{
			if (action->flags & type)
				action->setEnabled(true);
			else
				action->setEnabled(false);
		}
	}
}

