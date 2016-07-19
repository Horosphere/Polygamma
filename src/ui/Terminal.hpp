#ifndef _POLYGAMMA_UI_TERMINAL_HPP__
#define _POLYGAMMA_UI_TERMINAL_HPP__

#include <QKeyEvent>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QString>

#include "../core/Kernel.hpp"

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

/**
 * Terminal holds the event loop for the gui
 */
class Terminal final: public QMainWindow
{
	Q_OBJECT
public:
	explicit Terminal(Kernel* const, QWidget* parent = 0);

	void setScriptLevelMin(Script::Level) noexcept;
	bool getScriptLevelMin() const noexcept;
	void setFontMonospace(QFont, int tabWidth) noexcept;

Q_SIGNALS:
	void stdOutFlush(QString);
	void stdErrFlush(QString);

	void configUpdate();
	void bufferNew(Buffer*);
	void bufferErase(Buffer*);
	void bufferUpdate(Buffer*, Buffer::Update);

public Q_SLOTS:
	/**
	 * Sends a script to the Kernel.
	 */
	void onExecute(Script const&);

private Q_SLOTS:
	void eventLoop();

protected:
	virtual void closeEvent(QCloseEvent*) override;

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

	Script::Level scriptLevelMin;

	// Dynamic variables
	bool acceptingScripts; // Set to false when Kernel is busy
};


// Implementations

inline void Terminal::setScriptLevelMin(Script::Level level) noexcept
{
	scriptLevelMin = level;
}
inline bool Terminal::getScriptLevelMin() const noexcept
{
	return scriptLevelMin;
}
inline void Terminal::setFontMonospace(QFont font, int tabWidth) noexcept
{
	log->setFont(font);
	log->setTabStopWidth(tabWidth);
	input->setFont(font);
	input->setTabStopWidth(tabWidth);
}
inline void Terminal::closeEvent(QCloseEvent* event)
{
	this->hide();
	event->ignore();
}

} // namespace pg

#endif // !_POLYGAMMA_UI_TERMINAL_HPP__
