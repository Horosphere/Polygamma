#ifndef _POLYGAMMA_UI_UTIL_LINEEDITCOMMAND_HPP__
#define _POLYGAMMA_UI_UTIL_LINEEDITCOMMAND_HPP__

#include <QLineEdit>

#include "../../core/Command.hpp"

namespace pg
{

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

#endif // !_POLYGAMMA_UI_UTIL_LINEEDITCOMMAND_HPP__
