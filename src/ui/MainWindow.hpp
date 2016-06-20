#ifndef _POLYGAMMA_UI_MAINWINDOW_HPP__
#define _POLYGAMMA_UI_MAINWINDOW_HPP__

#include <QMainWindow>
#include <QStatusBar>
#include <QMenuBar>
#include <QLineEdit>

#include "Terminal.hpp"
#include "mainWindowAccessories.hpp"
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

Q_SIGNALS:
	// These signals are called in the Kernel thread, so connecting them requires
	// a queued connection.
	void stdOutFlush(QString);
	void stdErrFlush(QString);

private Q_SLOTS:
	// Currently deprecation does not work well with qt.
	// [[deprecated("Example method for summoning editor")]]
	void onFileImport();

	// Triggered upon configuration change
	void updateUIElements();

private:
	// Handlers
	Kernel* kernel;

	// UI Elements
	Terminal* terminal;
	LineEditCommand* lineEditCommand;
	QLineEdit* lineEditLog;
	QString lineEditLog_stylesheetOut;
	QString lineEditLog_stylesheetErr;

};

} // namespace pg

#endif // !_POLYGAMMA_UI_MAINWINDOW_HPP__

