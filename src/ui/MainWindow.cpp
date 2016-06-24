#include "MainWindow.hpp"

#include <QCloseEvent>
#include <QDir>
#include <QFileDialog>
#include <QFont>
#include <QMessageBox>
#include <QPushButton>
#include <QResource>
#include <QVBoxLayout>

#include "DialogPreferences.hpp"
#include "Terminal.hpp"


pg::MainWindow::MainWindow(Kernel* const kernel,
                           QWidget* parent): QMainWindow(parent),
	kernel(kernel), terminal(new Terminal(kernel, this)),
	lineEditCommand(new LineEditCommand), lineEditLog(new QLineEdit)
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
	QAction* actionFileImport = new QAction(tr("Import"), this);
	actionFileImport->setEnabled(false); // Temporarily disabled for debugging.
	menuFile->addAction(actionFileImport);
	// Menu Edit
	QMenu* menuEdit = menuBar()->addMenu(tr("Edit"));
	CommandAction* actionEditSummon = new CommandAction(Command("print('summon')"), "Summon", this);
	connect(actionEditSummon, &CommandAction::execute,
			terminal, &Terminal::onExecute);
	menuEdit->addAction(actionEditSummon);
	QAction* actionEditPreferences = new QAction(tr("Preferences..."), this);
	menuEdit->addAction(actionEditPreferences);
	// Menu end

	// Central
	QWidget* centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);
	QVBoxLayout* layoutMain = new QVBoxLayout(centralWidget);
	(void) layoutMain;

	// Status bar
	statusBar()->addPermanentWidget(lineEditCommand, 1);
	lineEditLog->setReadOnly(true);
	statusBar()->addPermanentWidget(lineEditLog, 1);
	QPushButton* buttonTerminal = new QPushButton;
	buttonTerminal->setIcon(QIcon(":/terminal.png"));
	buttonTerminal->setFixedSize(QSize(25,25));
	buttonTerminal->setIconSize(QSize(25,25));
	statusBar()->addPermanentWidget(buttonTerminal);
	statusBar()->show();

	// Connects the GUI.
	connect(buttonTerminal, &QPushButton::clicked,
	        this, [this]()
	{
		this->terminal->show();
	});

	// Menu actions
	connect(actionEditPreferences, &QAction::triggered,
	        this, [this]()
	{
		(new DialogPreferences(this->kernel, this))->show();
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
	connect(this, &MainWindow::stdOutFlush,
	        terminal->log, &TerminalLog::onStdOutFlush, Qt::QueuedConnection);
	connect(this, &MainWindow::stdErrFlush,
	        terminal->log, &TerminalLog::onStdErrFlush, Qt::QueuedConnection);
	// Command line
	connect(lineEditCommand, &LineEditCommand::execute,
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

	updateUIElements();

	// Set default states
	lineEditLog->setStyleSheet(lineEditLog_stylesheetOut);
	centralWidget->setMinimumSize(400, 100);
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
	lineEditCommand->setFont(fontMonospace);
	lineEditLog->setFont(fontMonospace);
	lineEditLog_stylesheetOut = QString("color: white; background-color: black");
	lineEditLog_stylesheetErr = QString("color: white; background-color: #220000");

	terminal->log->setFont(fontMonospace);
	terminal->log->setTabStopWidth(tabWidth);
	terminal->input->setFont(fontMonospace);
	terminal->input->setTabStopWidth(tabWidth);
}
