#ifndef _POLYGAMMA_UI_DIALOGS_DIALOGNEWSINGULAR_HPP__
#define _POLYGAMMA_UI_DIALOGS_DIALOGNEWSINGULAR_HPP__

#include <tuple>

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>

#include "../../singular/BufferSingular.hpp"

namespace pg
{

class DialogNewSingular final: public QDialog
{
	Q_OBJECT
public:
	explicit DialogNewSingular(QWidget* parent = 0);

	/**
	 * @return A tuple (channelLayout, sampleRate, duration)
	 */
	std::tuple<ChannelLayout, std::size_t, std::string> values() const noexcept;

private:
	QComboBox* comboChannelLayout;
	QComboBox* comboSampleRate;
	QLineEdit* lineEditDuration;
};

} // namespace pg


#endif // !_POLYGAMMA_UI_DIALOGS_DIALOGNEWSINGULAR_HPP__
