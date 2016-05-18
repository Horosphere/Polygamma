#ifndef PANELBASE_HPP
#define PANELBASE_HPP

#include <QActionGroup>
#include <QDockWidget>

namespace pg
{

/**
 * @brief The PanelBase class The base of all toolboxes in Polygamma.
 */
class PanelBase : public QDockWidget
{
	Q_OBJECT
public:
	/**
	 * @brief PanelBase Protected contructor for summoning panels.
	 * @param title The window title of this QDockWidget.
	 * @param parent The parent object. Will always be the MainWindow.
	 * @param flags See Qt::WindowFlags
	 */
	PanelBase(QWidget* parent = 0, Qt::WindowFlags flags = 0);

protected:

	/**
	 * Docking multiple
	 * @brief addDockActions Called by subclasses to add dock options to their
	 *        custom context menus.
	 */
	void addDockActions(QMenu* const);
protected Q_SLOTS:
	void onContextMenuRequest(QPoint);

private Q_SLOTS:

	void onDockLeft();
	void onDockRight();
	void onDockTop();
	void onDockBottom();

private:

};

}
#endif // PANELBASE_HPP
