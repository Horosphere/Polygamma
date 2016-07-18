#ifndef _POLYGAMMA_UI_DIALOGPREFERENCES_HPP__
#define _POLYGAMMA_UI_DIALOGPREFERENCES_HPP__

#include <QDialog>
#include <QComboBox>
#include <QListWidgetItem>
#include <QStackedWidget>

#include "../util/ColourButton.hpp"
#include "../../core/Configuration.hpp"

namespace pg
{

class DialogPreferences final: public QDialog
{
	Q_OBJECT
public:
	explicit DialogPreferences(Configuration* const, QWidget* parent = 0);

Q_SIGNALS:
	void configUpdate();

public Q_SLOTS:
	void onReload();

private Q_SLOTS:
	void save();

private:

	Configuration* const config;

	// The naming of the configurations must be consistent with
	// core/Configuration.hpp
	/*
	QComboBox* ioAudioDeviceInput;
	QComboBox* ioAudioDeviceOutput;
	*/

	ColourButton* uiBG;
	ColourButton* uiTerminalBG;
};

} // namespace pg

#endif // !_POLYGAMMA_UI_DIALOGPREFERENCES_HPP__
