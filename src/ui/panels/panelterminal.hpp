#ifndef PANELTERMINAL_HPP
#define PANELTERMINAL_HPP

#include <string>
#include <QPlainTextEdit>
#include <QTextEdit>

#include "panelbase.hpp"

namespace pg
{

class TerminalLog final: public QTextEdit
{
	Q_OBJECT
public:
	explicit TerminalLog(QWidget* parent = 0);

};

class TerminalInput final: public QPlainTextEdit
{
	Q_OBJECT
public:
	explicit TerminalInput(QWidget* parent = 0);

protected:
	void keyPressEvent(QKeyEvent*) override;

Q_SIGNALS:
	void execute(std::string);
};

class PanelTerminal final: public PanelBase
{

	Q_OBJECT
public:
	explicit PanelTerminal(QWidget *parent = 0);

Q_SIGNALS:

public Q_SLOTS:

private:
	TerminalLog* const log;
	TerminalInput* const input;
};

} // namespace pg

#endif // PANELTERMINAL_HPP
