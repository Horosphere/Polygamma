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
#include <QtAVWidgets>

#include "ui.hpp"
#include "Terminal.hpp"
#include "dialogs/DialogPreferences.hpp"
#include "dialogs/DialogNewSingular.hpp"
#include "editors/EditorSingular.hpp"


pg::MainWindow::MainWindow(Kernel* const kernel, Configuration* const config
, QWidget* parent): QMainWindow(parent),
	kernel(kernel), config(config),

	// UI
	terminal(new Terminal(kernel, this)),
	lineEditScript(new LineEditScript),
	lineEditLog(new QLineEdit),

	// Panels
	panelPlayback(new PanelPlayback(this)),
	panelMultimedia(new PanelMultimedia(this)),

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
#define ADD_ACTIONPANEL(name, string) \
	QAction* actionWindows##name = new QAction(tr(string), this); \
	connect(actionWindows##name, &QAction::triggered, \
	        panel##name, &Panel::show); \
	menuWindows->addAction(actionWindows##name);

	ADD_ACTIONPANEL(Playback, "Playback");
	ADD_ACTIONPANEL(Multimedia, "Multimedia");
#undef ADD_ACTIONPANEL

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
	        panelMultimedia, &PanelMultimedia::onPlayPause);
	// Script line
	connect(lineEditScript, &LineEditScript::execute,
	        terminal, &Terminal::onExecute);
	connect(terminal, &Terminal::stdOutFlush,
	        this, [this](QString const& str)
	{
		QStringList lines = str.split('\n', QString::SkipEmptyParts);
		if (lines.isEmpty()) return;
		lineEditLog->setStyleSheet(lineEditLog_stylesheetOut);
		lineEditLog->setText(lines.takeLast());
	});
	connect(terminal, &Terminal::stdErrFlush,
	        this, [this](QString const& str)
	{
		QStringList lines = str.split('\n', QString::SkipEmptyParts);
		if (lines.isEmpty()) return;
		lineEditLog->setText(lines.takeLast());
		lineEditLog->setStyleSheet(lineEditLog_stylesheetErr);
	});
	connect(terminal, &Terminal::configUpdate,
	        this, &MainWindow::updateUIElements);
	connect(dialogPreferences, &DialogPreferences::configUpdate,
	        this, &MainWindow::updateUIElements);
	connect(terminal, &Terminal::bufferNew,
	        this, &MainWindow::onBufferNew);
	connect(terminal, &Terminal::bufferErase,
	        this, &MainWindow::onBufferErase);
	connect(terminal, &Terminal::bufferUpdate,
	        this, &MainWindow::onBufferUpdate);

	updateUIElements();
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
	terminal->setFontMonospace(fontMonospace, tabWidth);

	terminal->setScriptLevelMin(config->uiScriptLevelMin);
	/*
	 * "Bool To String" Converts a bool to a QString for using the stylesheet.
	 */
	static auto bts = [](bool b) -> QString { return b ? "true" : "false"; };
	qApp->setStyleSheet(
	  "QMainWindow, QDialog, QDockWidget, QStatusBar {"
	  "background-color: " + abgrToString(config->uiBG) + ";"
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
	panelMultimedia->editorAdd(editor);

	connect(editor, &Editor::execute,
	        terminal, &Terminal::onExecute);
	connect(editor, &Editor::destroy,
	        this, [this, buffer, editor]()
	{
		this->editors.erase(editor);
		if (this->currentEditor == editor)
		this->currentEditor = nullptr;
		this->onExecute(PYTHON_KERNEL + QString(".eraseBuffer(") +
		                QString::number(kernel->bufferIndex(buffer)) + ')');
	});
	reloadMenuWindows();
}
void pg::MainWindow::onBufferErase(Buffer* buffer)
{
	for (auto editor: editors)
	{
		if (editor->getBuffer() != buffer) continue;

		qDebug() << "Found";

		if (editor == currentEditor)
			currentEditor = nullptr;
		delete editor;
		editors.erase(editor);
		panelMultimedia->editorErase(editor);
		reloadMenuWindows();
		break;
	}
	// Do not place an assert(false) here since the routine will reach this point
	// if the editor is closed using the close button
}
void pg::MainWindow::onBufferUpdate(Buffer* buffer, Buffer::Update update)
{
	for (auto& editor: editors)
	{
		if (editor->getBuffer() != buffer) continue;
		editor->update(update);
	}
}

void pg::MainWindow::onExecute(QString const& script)
{
	if (currentEditor)
	{
		// TODO: This is not very thread safe, as kernel->buffers may change.
		std::size_t index = kernel->bufferIndex(currentEditor->getBuffer());
		qDebug() << "Index " << index;
		QString s = script;
		s.replace("{CU}", QString(PYTHON_KERNEL) + ".buffers[" +
		          QString::number(index) + ']');

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
