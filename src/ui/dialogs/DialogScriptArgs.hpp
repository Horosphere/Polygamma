#ifndef _POLYGAMMA_UI_DIALOGS_DIALOGSCRIPTARGS_HPP__
#define _POLYGAMMA_UI_DIALOGS_DIALOGSCRIPTARGS_HPP__

#include <QDialog>
#include <QDialogButtonBox>

namespace pg
{

class DialogScriptArgs: public QDialog
{
	Q_OBJECT
public:
	explicit DialogScriptArgs(QWidget* parent = 0);

	virtual QString script() = 0;
protected:
	QDialogButtonBox* const buttonBox;

};

} // namespace pg


#endif // !_POLYGAMMA_UI_DIALOGS_DIALOGSCRIPTARGS_HPP__
