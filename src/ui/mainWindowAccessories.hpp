#ifndef _POLYGAMMA_UI_MAINWINDOWACCESSORIES_HPP__
#define _POLYGAMMA_UI_MAINWINDOWACCESSORIES_HPP__

#include <QAction>
#include <QLineEdit>

#include "../core/Script.hpp"

// Accessories for the MainWindow.
// Contents:
// LineEditScript: QLineEdit that emits a signal upon the key combination
//	Shift+Enter
namespace pg
{

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
