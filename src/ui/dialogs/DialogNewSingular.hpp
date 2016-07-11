#ifndef _POLYGAMMA_UI_DIALOGS_DIALOGNEWSINGULAR_HPP__
#define _POLYGAMMA_UI_DIALOGS_DIALOGNEWSINGULAR_HPP__

#include <tuple>

#include <QLineEdit>
#include <QComboBox>

#include "DialogScriptArgs.hpp"
#include "../../singular/BufferSingular.hpp"

namespace pg
{

class DialogNewSingular final: public DialogScriptArgs
{
	Q_OBJECT
public:
	explicit DialogNewSingular(QWidget* parent = 0);

	virtual QString script() override;
private:
	QComboBox* comboChannelLayout;
	QComboBox* comboSampleRate;
	QLineEdit* lineEditDuration;
};

} // namespace pg


#endif // !_POLYGAMMA_UI_DIALOGS_DIALOGNEWSINGULAR_HPP__
