#ifndef _POLYGAMMA_UI_TERMINAL_HPP__
#define _POLYGAMMA_UI_TERMINAL_HPP__

#include <QMainWindow>
#include <QPlainTextEdit>
#include <QKeyEvent>
#include <QString>

#include "../core/Kernel.hpp"

// The classes in this header implements a terminal that can be summoned in
// Polygamma.
namespace pg
{

class TerminalLog final: public QPlainTextEdit
{
	Q_OBJECT
public:
	explicit TerminalLog(QWidget* parent = 0);

private Q_SLOTS:
	void onLogUpdate(QString);

	friend class Terminal;
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
	// Called by void onLogUpdate(std::string) and converts std::string to
	// QString.
	void logUpdate(QString);

protected:
	virtual void closeEvent(QCloseEvent*) override;

private Q_SLOTS:

	void onExecution(Command const&);

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
};

} // namespace pg

#endif // !_POLYGAMMA_UI_TERMINAL_HPP__
