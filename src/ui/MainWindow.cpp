#include "MainWindow.hpp"

#include <iostream>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>
#include <QVBoxLayout>

#include "Terminal.hpp"
#include "editors/EditorSimple.hpp"

#include "../io/av.hpp"

pg::MainWindow::MainWindow(Kernel* const kernel,
                           QWidget* parent): QMainWindow(parent),
	kernel(kernel)
{

	setDockOptions(dockOptions() |
	               QMainWindow::AnimatedDocks |
	               QMainWindow::AllowTabbedDocks |
	               QMainWindow::GroupedDragging);
	//setTabPosition(Qt::RightDockWidgetArea, QTabWidget::East);
	setWindowTitle(tr("Polygamma"));

	// Menus
	QMenu* menuFile = menuBar()->addMenu(tr("File"));
	QAction* actionFileImport = new QAction("Import", this);
	actionFileImport->setEnabled(false); // Temporarily disabled for debugging.
	menuFile->addAction(actionFileImport);
	QMenu* menuEdit = menuBar()->addMenu(tr("Edit"));
	QAction* actionEditSummon = new QAction("Summon", this);
	menuEdit->addAction(actionEditSummon);

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
}
pg::MainWindow::~MainWindow()
{
	kernel->halt();
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
