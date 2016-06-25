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
	explicit MainWindow(Kernel* const, Configuration* const,
			QWidget* parent = 0);

protected:
	virtual void closeEvent(QCloseEvent*) override;

Q_SIGNALS:
	// These signals are called in the Kernel thread, so connecting them requires
	// a queued connection.
	void stdOutFlush(QString);
	void stdErrFlush(QString);
	void newBuffer(Buffer*);

private Q_SLOTS:

	// Triggered upon configuration change
	void updateUIElements();
	void onNewBuffer(Buffer*);

private:
	// Handlers
	Kernel* const kernel;
	Configuration* const config;

	// UI Elements
	Terminal* terminal;
	LineEditCommand* lineEditCommand;
	QLineEdit* lineEditLog;
	QString lineEditLog_stylesheetOut;
	QString lineEditLog_stylesheetErr;

};

} // namespace pg

#endif // !_POLYGAMMA_UI_MAINWINDOW_HPP__

