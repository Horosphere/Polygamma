#ifndef _POLYGAMMA_UI_MAINWINDOW_HPP__
#define _POLYGAMMA_UI_MAINWINDOW_HPP__

#include <vector>

#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QStatusBar>

#include "Terminal.hpp"
#include "mainWindowAccessories.hpp"
#include "editors/Editor.hpp"
#include "util/actions.hpp"
#include "dialogs/DialogPreferences.hpp"
#include "dialogs/DialogNewSingular.hpp"
#include "../core/Kernel.hpp"

namespace pg
{

class MainWindow final: public QMainWindow
{
	Q_OBJECT
public:
	explicit MainWindow(Kernel* const, Configuration* const,
			QWidget* parent = 0);

protected:
	virtual void closeEvent(QCloseEvent*) override;

Q_SIGNALS:
	// These signals are called in the Kernel thread, so connecting them requires
	// a queued connection.
	void stdOutFlush(QString);
	void stdErrFlush(QString);
	void bufferNew(Buffer*);
	/**
	 * The argument must be an element of std::vector<Editor*> editors.
	 */
	void bufferDestroy(Editor*);

private Q_SLOTS:

	// Triggered upon configuration change
	void updateUIElements();
	void onBufferNew(Buffer*);
	/**
	 * The argument must be an element of std::vector<Editor*> editors.
	 */
	void onBufferDestroy(Editor*);
	/**
	 * The following combinations will be replaced
	 * {CU} -> The current buffer. e.g. pg.kernel.buffers[4]
	 *
	 * @warning Must be called with at least one active buffer present.
	 * @brief Converts a QString into a system level script and calls 
	 *	Terminal::onExecute to send it to the Kernel.
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
	QMenu* menuWindows;
	/**
	 * @brief Stores all actionFlagged that are deactivated upon particular editor
	 *	changes.
	 */
	std::vector<ActionFlagged*> actionsFlagged;

	Terminal* terminal;
	LineEditScript* lineEditScript;
	QLineEdit* lineEditLog;

	// Dialogs
	DialogPreferences* const dialogPreferences;
	DialogNewSingular* const dialogNewSingular;

	// Dynamic
	/**
	 * @brief The index is editors.size() when no editor is selected.
	 */
	std::size_t currentEditorIndex;
	/**
	 * @warning Do not use editor index to index buffers in the Kernel.
	 * @brief Each editor corresponds to a buffer in the Kernel or a subbuffer.
	 */
	std::vector<Editor*> editors;
};

} // namespace pg


#endif // !_POLYGAMMA_UI_MAINWINDOW_HPP__
