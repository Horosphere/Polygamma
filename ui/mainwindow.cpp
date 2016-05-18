#include "mainwindow.hpp"

#include <QFileDialog>
#include <QDir>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QDebug>

#include "editors/editorsimple.hpp"
#include "../io/av.hpp"

pg::MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent),
	actionFileImport(new QAction("Import", this)),
	actionEditSummon(new QAction("Summon", this))
{
	setDockOptions(dockOptions() |
				   QMainWindow::AnimatedDocks |
				   QMainWindow::AllowTabbedDocks |
				   QMainWindow::GroupedDragging);
	//setTabPosition(Qt::RightDockWidgetArea, QTabWidget::East);
	setWindowTitle(tr("Polygamma"));

	// Menus
	QMenu* menuFile = menuBar()->addMenu(tr("File"));
	menuFile->addAction(actionFileImport);
	QMenu* menuEdit = menuBar()->addMenu(tr("Edit"));
	menuEdit->addAction(actionEditSummon);

	QWidget* centralWidget = new QWidget(this);
	setCentralWidget(centralWidget);
	centralWidget->setMinimumSize(400, 400);
	QVBoxLayout* layoutMain = new QVBoxLayout(centralWidget);
	Q_UNUSED(layoutMain)

	statusBar()->show();

	// Connects the GUI.
	// Each QAction* action[menu][name] field in class MainWindow
	// corresponds to a method void on[menu][name] of MainWindow.
	connect(actionFileImport, &QAction::triggered, this, &MainWindow::onFileImport);
	connect(actionEditSummon, &QAction::triggered, this, &MainWindow::onEditSummon);
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
}
