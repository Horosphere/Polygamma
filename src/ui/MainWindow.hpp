#ifndef _POLYGAMMA_UI_MAINWINDOW_HPP__
#define _POLYGAMMA_UI_MAINWINDOW_HPP__

#include <QMainWindow>
#include <QStatusBar>
#include <QMenuBar>

#include "Terminal.hpp"
#include "../core/Kernel.hpp"

namespace pg
{

class MainWindow final: public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(Kernel* const, QWidget* parent = 0);

protected:
	virtual void closeEvent(QCloseEvent*) override;

public Q_SLOTS:

private Q_SLOTS:
	void onFileImport();
	void onEditSummon();
	void onEditPreferences();

private:
	// Handlers
	Kernel* kernel;

	// UI Elements
	Terminal* terminal;

};

} // namespace pg

#endif // !_POLYGAMMA_UI_MAINWINDOW_HPP__

