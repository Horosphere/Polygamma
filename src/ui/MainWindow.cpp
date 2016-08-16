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


namespace pg
{

MainWindow::MainWindow(Kernel* const kernel, Configuration* const config
, QWidget* parent): QMainWindow(parent),
	kernel(kernel), config(config),

	// UI
	terminal(new Terminal(kernel, this)),
	lineEditScript(new LineEditScript),
	lineEditLog(new QLineEdit),

	// Panels
	panelPlayback(new PanelPlayback(this)),

	// Dialogs
	dialogPreferences(new DialogPreferences(config, this)),
	dialogNewSingular(new DialogNewSingular(this))

	// Dynamics
{
	setFocusPolicy(Qt::NoFocus);
	setWindowIcon(QIcon(":/icon.png"));
	setDockOptions(dockOptions() |
	               QMainWindow::AnimatedDocks |
	               QMainWindow::AllowTabbedDocks);
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
			this->onExecute("{CU}.saveToFile('" + fileName + "')");
	});
	connect(actionEditPreferences, &QAction::triggered,
	        this, [this]()
	{
		dialogPreferences->onReload();
		dialogPreferences->show();
	});

	// Panels
	panelPlayback->hide();

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
	setCurrentEditor(nullptr);
	lineEditLog->setStyleSheet(lineEditLog_stylesheetOut);
	terminal->setBaseSize(QSize(300, 500));
	// Disable all actions since nothing is loaded
	for (auto& action: actionsFlagged) action->setEnabled(false);
}

void MainWindow::updateUIElements()
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

void MainWindow::onBufferNew(Buffer const* buffer)
{
	Editor* editor;

	if (auto const* b = dynamic_cast<BufferSingular const*>(buffer))
	{
		editor = new EditorSingular(kernel, b, this);
	}
	else
	{
		qDebug() << "[UI] Unrecognised Buffer Type";
		assert(false && "Buffer type unrecognised by MainWindow");
	}
	editor->show();
	//editor->setFloating(true);
	editors.insert(editor);

	connect(editor, &Editor::execute,
	        terminal, &Terminal::onExecute);
	connect(editor, &Editor::destroy,
	        this, [this, buffer, editor]()
	{
		assert(editor->getBuffer() == buffer);

		this->editors.erase(editor);
		if (this->currentEditor == editor)
			this->setCurrentEditor(nullptr);
		QString index = QString::number(kernel->bufferIndex(buffer));
		this->onExecute(PYTHON_KERNEL + QString(".eraseBuffer(") +
		                index + ')');
	});

	reloadMenuWindows();
}
void MainWindow::onBufferErase(Buffer const* buffer)
{
	if (Editor* editor = editorFromBuffer(buffer))
	{
		if (editor == currentEditor)
			setCurrentEditor(nullptr);
		delete editor;
		editors.erase(editor);
		reloadMenuWindows();
	}
	// Do not place an assert(false) here since the routine will reach this point
	// if the editor is closed using the close button
}
void MainWindow::onBufferUpdate(Buffer const* buffer, Buffer::Update update)
{
	if (Editor* editor = editorFromBuffer(buffer))
	{
		editor->update(update);
	}
}

void MainWindow::onExecute(QString const& script)
{
	if (currentEditor)
	{
		// TODO: This is not very thread safe, as kernel->buffers may change.
		std::size_t index = kernel->bufferIndex(currentEditor->getBuffer());
		QString s = script;
		s.replace("{CU}", QString(PYTHON_KERNEL) + ".buffers[" +
		          QString::number(index) + ']');

		terminal->onExecute(Script(s.toStdString()));
	}
	else
		terminal->onExecute(Script(script.toStdString()));
}

void MainWindow::onFocusChanged(QWidget* old, QWidget* now)
{
	(void) old;
	if (Editor* editor = dynamic_cast<Editor*>(now))
	{
		currentEditor = editor;
		panelPlayback->setEnabled(true);
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
void MainWindow::reloadMenuWindows()
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

} // namespace pg
