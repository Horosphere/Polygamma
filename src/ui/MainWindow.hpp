#ifndef _POLYGAMMA_UI_MAINWINDOW_HPP__
#define _POLYGAMMA_UI_MAINWINDOW_HPP__

#include <set>

#include <QLineEdit>
#include <QMainWindow>

#include "Terminal.hpp"
#include "dialogs/DialogNewSingular.hpp"
#include "dialogs/DialogPreferences.hpp"
#include "editors/Editor.hpp"
#include "panels/PanelPlayback.hpp"
#include "panels/PanelMultimedia.hpp"
#include "mainWindowAccessories.hpp"
#include "util/actions.hpp"
#include "../core/Kernel.hpp"

namespace pg
{

class MainWindow final: public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(Kernel* const, Configuration* const,
	                    QWidget* parent = 0);

private Q_SLOTS:

	// Triggered upon configuration change
	void updateUIElements();
	void onBufferNew(Buffer*);
	void onBufferErase(Buffer*);
	void onBufferUpdate(Buffer*, Buffer::Update);

	/**
	 * The following combinations will be replaced
	 * {CU} -> The current buffer. e.g. pg.kernel.buffers[4]
	 *
	 * @warning Must be called with at least one active buffer present.
	 * @brief Converts a QString into a system level script and calls
	 *  Terminal::onExecute to send it to the Kernel.
	 */
	void onExecute(QString const&);

	void onFocusChanged(QWidget* old, QWidget* now);
private:
	/**
	 * @brief Update menuWindows so its actions match the editors
	 */
	void reloadMenuWindows();

	// Handlers
	Kernel* const kernel;
	Configuration* const config;

	QString lineEditLog_stylesheetOut;
	QString lineEditLog_stylesheetErr;
	// UI Elements
	QMenu* menuEditors;
	/**
	 * @brief Stores all actionFlagged that are deactivated upon particular editor
	 *  changes.
	 */
	std::vector<ActionFlagged*> actionsFlagged;

	Terminal* terminal;
	LineEditScript* lineEditScript;
	QLineEdit* lineEditLog;

	// Panels
	PanelPlayback* const panelPlayback;
	PanelMultimedia* const panelMultimedia;

	// Dialogs
	DialogPreferences* const dialogPreferences;
	DialogNewSingular* const dialogNewSingular;

	// Dynamic
	/**
	 * nullptr when no editor is selected
	 */
	Editor* currentEditor;
	/**
	 * @brief Each editor corresponds to a buffer in the Kernel or a subbuffer.
	 */
	std::set<Editor*> editors;
};

} // namespace pg


#endif // !_POLYGAMMA_UI_MAINWINDOW_HPP__
