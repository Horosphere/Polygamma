#ifndef _POLYGAMMA_UI_MAINWINDOWACCESSORIES_HPP__
#define _POLYGAMMA_UI_MAINWINDOWACCESSORIES_HPP__

#include <QAction>
#include <QLineEdit>

#include "../core/Script.hpp"

// Accessories for the MainWindow.
// Contents:
// ScriptAction: QAction that has an associated command
// LineEditScript: QLineEdit that emits a signal upon the key combination
//	Shift+Enter
namespace pg
{

class ScriptAction final: public QAction
{
	Q_OBJECT
public:
	ScriptAction(Script const&, QObject* parent = 0);
	ScriptAction(Script const&, QString const&, QObject* parent = 0);
	ScriptAction(Script const&, QIcon const&, QString const&, QObject* parent = 0);

	Script command;
Q_SIGNALS:
	void execute(Script const&);
};

class LineEditScript final: public QLineEdit
{
	Q_OBJECT
public:
	explicit LineEditScript(QWidget* parent = 0);

Q_SIGNALS:
	void execute(Script);

protected:
	virtual void keyPressEvent(QKeyEvent*) override;

};


} // namespace pg

#endif // !_POLYGAMMA_UI_MAINWINDOWACCESSORIES_HPP__
