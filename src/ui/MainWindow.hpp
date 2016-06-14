#ifndef _POLYGAMMA_UI_MAINWINDOW_HPP__
#define _POLYGAMMA_UI_MAINWINDOW_HPP__

#include <QMainWindow>
#include <QStatusBar>
#include <QMenuBar>

#include "../core/Kernel.hpp"

namespace pg
{

class MainWindow final: public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(Kernel* const, QWidget* parent = 0);
	~MainWindow();


public Q_SLOTS:

private Q_SLOTS:
	void onFileImport();
	void onEditSummon();

private:
	// UI Elements

	// Handlers
	Kernel* kernel;
};

} // namespace pg

#endif // !_POLYGAMMA_UI_MAINWINDOW_HPP__

