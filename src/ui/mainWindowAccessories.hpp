#ifndef _POLYGAMMA_UI_MAINWINDOWACCESSORIES_HPP__
#define _POLYGAMMA_UI_MAINWINDOWACCESSORIES_HPP__

#include <QAction>
#include <QLineEdit>

#include "dialogs/DialogScriptArgs.hpp"
#include "util/actions.hpp"
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

/**
 * @brief \ref ActionFlagged with a \ref DialogScriptArgs. When triggered,
 *	dialog->exec() is called and if the user accepts, the signal
 *	execute(dialog->script()) is emitted.
 */
class ActionDialog: public ActionFlagged
{
	Q_OBJECT
public:
	explicit ActionDialog(DialogScriptArgs* const dialog, QObject* parent = 0);
	explicit ActionDialog(DialogScriptArgs* const dialog, QString const& text,
	                        QObject* parent = 0);
	explicit ActionDialog(DialogScriptArgs* const dialog, QIcon const&,
	                        QString const& text, QObject* parent = 0);

	DialogScriptArgs* const dialog;

Q_SIGNALS:
	void execute(QString const&);

};

} // namespace pg

#endif // !_POLYGAMMA_UI_MAINWINDOWACCESSORIES_HPP__
