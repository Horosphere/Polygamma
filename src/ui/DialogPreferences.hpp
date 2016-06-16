#ifndef _POLYGAMMA_UI_DIALOGPREFERENCES_HPP__
#define _POLYGAMMA_UI_DIALOGPREFERENCES_HPP__

#include <QDialog>
#include <QComboBox>
#include <QListWidgetItem>
#include <QStackedWidget>

#include "util/ColourButton.hpp"
#include "../core/Kernel.hpp"

namespace pg
{

class DialogPreferences final: public QDialog
{
	Q_OBJECT
public:
	explicit DialogPreferences(Kernel* const, QWidget* parent = 0);

protected:
	virtual void closeEvent(QCloseEvent*) override;

public Q_SLOTS:
	void onReload();

private:
	void save();

	Kernel* const kernel;

	// The naming of the configurations must be consistent with
	// core/Configuration.hpp
	ColourButton* uiColourBG;
	ColourButton* uiTerminalBG;
};
} // namespace pg

#endif // !_POLYGAMMA_UI_DIALOGPREFERENCES_HPP__
