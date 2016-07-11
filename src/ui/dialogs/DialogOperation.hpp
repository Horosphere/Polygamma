#ifndef _POLYGAMMA_UI_DIALOGS_DIALOGOPERATION_HPP__
#define _POLYGAMMA_UI_DIALOGS_DIALOGOPERATION_HPP__

#include <QDialog>
#include <QDialogButtonBox>

namespace pg
{

/**
 * Base class for all dialogs with a single Ok button on the bottom.
 */
class DialogOperation: public QDialog
{
	Q_OBJECT
public:
	explicit DialogOperation(QWidget* parent = 0);


protected:
	QDialogButtonBox* const buttonBox;

};

} // namespace pg


#endif // !_POLYGAMMA_UI_DIALOGS_DIALOGOPERATION_HPP__
