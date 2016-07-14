#include "MainWindow.hpp"

#include <QApplication>
#include <QCloseEvent>
#include <QMenuBar>
#include <QStatusBar>
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
	kernel(kernel), config(config), multimediaEngine(this),

	// UI
	terminal(new Terminal(kernel, this)),
	lineEditScript(new LineEditScript),
	lineEditLog(new QLineEdit),

	// Panels
	panelPlayback(new PanelPlayback(this)),

	// Dialogs
	dialogPreferences(new DialogPreferences(config, this)),
	dialogNewSingular(new DialogNewSingular(this)),

	// Dynamics
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
// Also used for ActionDialog
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

	ActionDialog* actionFileNewSingular = new ActionDialog(dialogNewSingular,
	    tr("Singular..."),
	    this);
	connect(actionFileNewSingular, &ActionDialog::execute,
	        this, &MainWindow::onExecute);
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

	ActionScripted* actionEditSilence = new ActionScripted("pg.silence({CU})", "Silence", this);
	ADD_ACTIONSCRIPTED(menuEdit, actionEditSilence, Buffer::Singular);
	QAction* actionEditPreferences = new QAction(tr("Preferences..."), this);
	menuEdit->addAction(actionEditPreferences);

	QMenu* menuWindows = menuBar()->addMenu(tr("Windows"));
	QAction* actionWindowsPlayback = new QAction(tr("Playback"), this);
	connect(actionWindowsPlayback, &QAction::triggered,
	        panelPlayback, &Panel::show);
	menuWindows->addAction(actionWindowsPlayback);
	menuEditors = menuWindows->addMenu(tr("Buffers"));

	// Menu end
#undef ADD_ACTIONFLAGGED
#undef ADD_ACTIONSCRIPTED

	// Status bar
	statusBar()->addPermanentWidget(lineEditScript, 1);
	lineEditLog->setReadOnly(true);
	statusBar()->addPermanentWidget(lineEditLog, 1);
	QPushButton* buttonTerminal = new QPushButton;
	buttonTerminal->setIcon(QIcon(":/terminal.png"));
	buttonTerminal->setFixedSize(QSize(25, 25));
	buttonTerminal->setIconSize(QSize(25, 25));
	statusBar()->addPermanentWidget(buttonTerminal);
	statusBar()->show();

	// Connects the GUI.
	config->registerUpdateListener([this]()
	{
		this->updateUIElements();
		this->multimediaEngine.loadConfiguration(this->config);
	});
	connect(qApp, &QApplication::focusChanged,
	        this, &MainWindow::onFocusChanged);
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
		dialogPreferences->onReload();
		dialogPreferences->show();
	});

	// Panels
	connect(panelPlayback, &PanelPlayback::playPause,
	        this, [this]()
	{
		// TODO: There are many problems to this design:
		// Multiple playbacks can cause the application to crash
		// there is no way to stop
		// Crashes if currentEditor = nullptr
		this->multimediaEngine.startPlayback(this->currentEditor);
	});
	// Connects the kernel's stdout and stderr to the corresponding signals of
	// this class
	kernel->registerStdOutListener([this](std::string str)
	{
		Q_EMIT this->stdOutFlush(QString::fromStdString(str));
	});
	kernel->registerStdErrListener([this](std::string str)
	{
		Q_EMIT this->stdErrFlush(QString::fromStdString(str));
	});
	kernel->registerBufferListener([this](Buffer* buffer)
	{
		Q_EMIT this->bufferNew(buffer);
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
	connect(this, &MainWindow::bufferNew,
	        this, &MainWindow::onBufferNew, Qt::QueuedConnection);
	connect(this, &MainWindow::bufferDestroy,
	        this, &MainWindow::onBufferDestroy, Qt::QueuedConnection);

	updateUIElements();
	multimediaEngine.loadConfiguration(config);
	reloadMenuWindows();

	// Set default states
	setBaseSize(QSize(300, 500));
	lineEditLog->setStyleSheet(lineEditLog_stylesheetOut);
	terminal->setBaseSize(QSize(300, 500));
	// Disable all actions since nothing is loaded
	for (auto& action: actionsFlagged) action->setEnabled(false);
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

void pg::MainWindow::onBufferNew(Buffer* buffer)
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
		assert(false && "Buffer type unrecognised by MainWindow");
	}
	editor->show();
	//editor->setFloating(true);
	editors.insert(editor);
	multimediaEngine.addBuffer(editor);

	buffer->registerUIDestroyListener([this, editor]()
	{
		Q_EMIT this->bufferDestroy(editor);
	});
	connect(editor, &Editor::execute,
	        terminal, &Terminal::onExecute);
	reloadMenuWindows();
}
void pg::MainWindow::onBufferDestroy(Editor* editor)
{
	if (editor == currentEditor)
		currentEditor = nullptr;
	delete editor;
	editors.erase(editor);
	multimediaEngine.eraseBuffer(editor);
	reloadMenuWindows();
}

void pg::MainWindow::onExecute(QString const& script)
{
	if (currentEditor)
	{
		// TODO: This is not very thread safe, as kernel->buffers may change.
		std::size_t index = kernel->bufferIndex(currentEditor->getBuffer());
		QString s = script;
		s.replace("{CU}", QString(PYTHON_KERNEL) + '(' +
		          QString::number(index) + ')');

		terminal->onExecute(Script(s.toStdString()));
	}
	else
		terminal->onExecute(Script(script.toStdString()));
}

void pg::MainWindow::onFocusChanged(QWidget* old, QWidget* now)
{
	(void) old;
	if (Editor* editor = dynamic_cast<Editor*>(now))
	{
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
void pg::MainWindow::reloadMenuWindows()
{
	menuEditors->clear();
	for (auto const& editor: editors)
	{
		QAction* actionEditor = new QAction(editor->windowTitle(), this);
		connect(actionEditor, &QAction::triggered,
		        editor, &QWidget::show);
		menuEditors->addAction(actionEditor);
	}
}
