#include "MainWindow.hpp"

#include <iostream>

#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QCloseEvent>
#include <QResource>

#include "Terminal.hpp"
#include "DialogPreferences.hpp"
#include "editors/EditorSimple.hpp"

#include "../io/av.hpp"

pg::MainWindow::MainWindow(Kernel* const kernel,
                           QWidget* parent): QMainWindow(parent),
	kernel(kernel)
{
	setWindowIcon(QIcon(":/icon.png"));
	setDockOptions(dockOptions() |
	               QMainWindow::AnimatedDocks |
	               QMainWindow::AllowTabbedDocks |
	               QMainWindow::GroupedDragging);
	//setTabPosition(Qt::RightDockWidgetArea, QTabWidget::East);
	setWindowTitle(tr("Polygamma"));

	// Menus
	// Menu File
	QMenu* menuFile = menuBar()->addMenu(tr("File"));
	QAction* actionFileImport = new QAction(tr("Import"), this);
	actionFileImport->setEnabled(false); // Temporarily disabled for debugging.
	menuFile->addAction(actionFileImport);
	// Menu Edit
	QMenu* menuEdit = menuBar()->addMenu(tr("Edit"));
	QAction* actionEditSummon = new QAction("Summon", this);
	menuEdit->addAction(actionEditSummon);
	QAction* actionEditPreferences = new QAction(tr("Preferences..."), this);
	menuEdit->addAction(actionEditPreferences);

	// Central
	QWidget* centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);
	centralWidget->setMinimumSize(400, 400);
	QVBoxLayout* layoutMain = new QVBoxLayout(centralWidget);
	(void) layoutMain;

	statusBar()->show();

	// Connects the GUI.
	// Each QAction* action[menu][name] field in class MainWindow
	// corresponds to a method void on[menu][name] of MainWindow.
	connect(actionFileImport, &QAction::triggered,
	        this, &MainWindow::onFileImport);
	connect(actionEditSummon, &QAction::triggered,
	        this, &MainWindow::onEditSummon);
	connect(actionEditPreferences, &QAction::triggered,
			this, &MainWindow::onEditPreferences);
}

void pg::MainWindow::closeEvent(QCloseEvent* event)
{
	kernel->halt();
	QMainWindow::closeEvent(event);
}

void pg::MainWindow::onFileImport()
{
	QString error = QString();
	EditorSimple* editor = EditorSimple::fromFile(this, &error);
	if (!editor)
	{
		if (error.isNull()) return; // The user did not choose a file.
		QMessageBox::critical(this, tr("Error"), error, QMessageBox::NoButton);
		return;
	}
	addDockWidget(Qt::LeftDockWidgetArea, editor);
}
void pg::MainWindow::onEditSummon()
{
	Terminal* terminal = new Terminal(kernel, this);
	terminal->show();
}

void pg::MainWindow::onEditPreferences()
{
	DialogPreferences* dp = new DialogPreferences(kernel, this);
	dp->show();
}
