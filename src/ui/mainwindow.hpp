#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>
#include <QStatusBar>
#include <QMenuBar>

namespace pg
{

class MainWindow final: public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(QWidget* parent = 0);


public Q_SLOTS:

private Q_SLOTS:
	void onFileImport();
	void onEditSummon();

private:
	// UI Elements

	// Menu actions
	QAction* actionFileImport;
	QAction* actionEditSummon;
	// Handlers
};

} // namespace pg

#endif // MAINWINDOW_HPP
