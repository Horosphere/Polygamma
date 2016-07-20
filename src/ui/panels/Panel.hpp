#ifndef _POLYGAMMA_UI_PANELS_PANEL_HPP__
#define _POLYGAMMA_UI_PANELS_PANEL_HPP__

#include <vector>

#include <QActionGroup>
#include <QDockWidget>

#include "../../core/Configuration.hpp"

namespace pg
{

/**
 * When the close button is triggered, panels turn hidden instead of being
 * destroyed.
 * @brief Base of all panels. Does not have a menu bar or a status bar.
 */
class Panel: public QDockWidget
{
	Q_OBJECT
public:
	/**
	 * @brief PanelBase Protected contructor for summoning panels.
	 * @param title The window title of this QDockWidget.
	 * @param parent The parent object. Will always be the MainWindow.
	 * @param flags See Qt::WindowFlags
	 */
	Panel(QWidget* parent = 0, Qt::WindowFlags flags = 0);

public Q_SLOTS:
	void show();
	void setSubwidgetsEnabled(bool);

protected:
	virtual void closeEvent(QCloseEvent*) final override;

	/**
	 * @brief setEnabled(bool) of the widget will be called by
	 *	setSubwidgetsEnabled(bool)
	 */
	void addSubwidget(QWidget*) noexcept;
	/**
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
	std::vector<QWidget*> subwidgets;
};


// Implementations

inline void Panel::addSubwidget(QWidget* widget) noexcept
{
	subwidgets.push_back(widget);
}

} // namespace pg


#endif // !_POLYGAMMA_UI_PANELS_PANEL_HPP__
