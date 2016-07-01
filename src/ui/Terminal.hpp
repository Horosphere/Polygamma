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
	void execute(Script);

private Q_SLOTS:
	void onPreserveInputToggled(bool);

private:
	bool preserveInput;

	friend class Terminal;
};

class Terminal final: public QMainWindow
{
	Q_OBJECT
	Q_PROPERTY(bool showSystemLevel READ getShowSystemLevel WRITE setShowSystemLevel)
public:
	explicit Terminal(Kernel* const, QWidget* parent = 0);

	void setShowSystemLevel(bool) noexcept;
	bool getShowSystemLevel() const noexcept;
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
	 * Sends a script to the Kernel.
	 */
	void onExecute(Script const&);


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

	bool showSystemLevel;

	friend class MainWindow;
};

} // namespace pg

// Implementations

inline void
pg::Terminal::setShowSystemLevel(bool flag) noexcept
{
	showSystemLevel = flag;
}
inline bool
pg::Terminal::getShowSystemLevel() const noexcept
{
	return showSystemLevel;
}
#endif // !_POLYGAMMA_UI_TERMINAL_HPP__
