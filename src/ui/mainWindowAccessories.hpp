#ifndef _POLYGAMMA_UI_MAINWINDOWACCESSORIES_HPP__
#define _POLYGAMMA_UI_MAINWINDOWACCESSORIES_HPP__

#include <QAction>
#include <QLineEdit>

#include "../core/Command.hpp"

// Accessories for the MainWindow.
// Contents:
// CommandAction: QAction that has an associated command
// LineEditCommand: QLineEdit that emits a signal upon the key combination
//	Shift+Enter
namespace pg
{

class CommandAction final: public QAction
{
	Q_OBJECT
public:
	CommandAction(Command const&, QObject* parent = 0);
	CommandAction(Command const&, QString const&, QObject* parent = 0);
	CommandAction(Command const&, QIcon const&, QString const&, QObject* parent = 0);

	Command command;
Q_SIGNALS:
	void execute(Command const&);
};

class LineEditCommand final: public QLineEdit
{
	Q_OBJECT
public:
	explicit LineEditCommand(QWidget* parent = 0);

Q_SIGNALS:
	void execute(Command);

protected:
	virtual void keyPressEvent(QKeyEvent*) override;

};


} // namespace pg

#endif // !_POLYGAMMA_UI_MAINWINDOWACCESSORIES_HPP__
