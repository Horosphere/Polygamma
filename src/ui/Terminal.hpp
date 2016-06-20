#ifndef _POLYGAMMA_UI_TERMINAL_HPP__
#define _POLYGAMMA_UI_TERMINAL_HPP__

#include <QKeyEvent>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QString>

#include "../core/Kernel.hpp"

// The classes in this header implements a terminal that can be summoned in
// Polygamma.
namespace pg
{

class TerminalLog final: public QTextEdit
{
	Q_OBJECT
public:
	explicit TerminalLog(QWidget* parent = 0);

public Q_SLOTS:
	void onStdOutFlush(QString);
	void onStdErrFlush(QString);

};
class TerminalInput final: public QPlainTextEdit
{
	Q_OBJECT
public:
	explicit TerminalInput(QWidget* parent = 0);

protected:
	virtual void keyPressEvent(QKeyEvent*) override;

Q_SIGNALS:
	void execute(Command);
};

class Terminal final: public QMainWindow
{
	Q_OBJECT
public:
	
	
	explicit Terminal(Kernel* const, QWidget* parent = 0);

Q_SIGNALS:
	/**
	 * @brief Receive the updated log from the Kernel. This signal is emitted in
	 * the Kernel thread, so all connections to it must be queued.
	 */
	void logUpdate(QString);

protected:
	virtual void closeEvent(QCloseEvent*) override;

private Q_SLOTS:
	/**
	 * Sends a command to the Kernel.
	 */
	void onExecute(Command const&);

private:
	/**
	 * @brief Called when the log has been updated. This slot is connected to
	 * the Kernel and executed in the kernel thread. It relays the signal to
	 * void logUpdate(QString);
	 */
	void onLogUpdate(std::string);

	TerminalLog* log;
	TerminalInput* input;
	Kernel* kernel;

	friend class MainWindow;
};

} // namespace pg

#endif // !_POLYGAMMA_UI_TERMINAL_HPP__
